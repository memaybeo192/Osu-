#pragma once
#include <Windows.h>
#include <iostream>
#include <deque>
#include <thread>
#include <chrono>
#include "Config.h"
#include "KeyMap.h"
#include "Detection.h"
#include "InputControl.h"

// Kiểm tra cửa sổ Osu! đang active không
bool isWindowActive(const wchar_t* title) {
    HWND hwnd = GetForegroundWindow();
    wchar_t buff[256];
    GetWindowTextW(hwnd, buff, 256);
    return wcsstr(buff, title) != nullptr;
}

// Kiểm tra vùng scan có bất kỳ nốt nào (head hoặc hold) không
bool isAnyNotePresentInScanRegion(HDC hdc, int tol, int boxSize) {
    for (auto& [ch, nk] : keyMap) {
        for (int dx = -boxSize; dx <= boxSize; dx++) {
            for (int dy = -boxSize; dy <= boxSize; dy++) {
                int x = nk.relativePos.x + dx;
                int y = nk.relativePos.y + dy;
                if (x < 0 || x >= SCAN_REGION_WIDTH || y < 0 || y >= SCAN_REGION_HEIGHT) 
                    continue;
                COLORREF c = GetPixel(hdc, x, y);
                if (c != CLR_INVALID &&
                   (isColorNear(c, NOTE_COLOR, tol) ||
                    isColorNear(c, HOLD_BODY_COLOR, HOLD_TOLERANCE)))
                    return true;
            }
        }
    }
    return false;
}

// Vòng lặp bot
void botLoop(HDC hdcScreen, HDC hdcMem, HBITMAP hbmScreen) {
    std::deque<std::chrono::steady_clock::time_point> hits;
    bool botActive = false;
    bool isDenseMode = false;
    bool inGamePlay = false;

    std::cout << "[INFO] Bot INACTIVE. Nhấn PgUp để toggle.\n";

    while (true) {
        auto now = std::chrono::steady_clock::now();

        // --- Toggle ON/OFF ---
        if (GetAsyncKeyState(TOGGLE_HOTKEY) & 0x8000) {
            botActive = !botActive;
            std::cout << "[INFO] Bot " << (botActive ? "ON\n" : "OFF\n");
            // Nếu off: nhả hết phím
            if (!botActive) {
                for (auto& [ch, nk] : keyMap)
                    if (nk.isHolding) {
                        release(nk.virtualKey);
                        nk.isHolding = false;
                    }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        if (!botActive) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        // --- Bắt đầu capture vùng chơi ---
        BitBlt(hdcMem, 0, 0, SCAN_REGION_WIDTH, SCAN_REGION_HEIGHT,
               hdcScreen, SCAN_REGION_X, SCAN_REGION_Y, SRCCOPY);

        // --- Dense mode update ---
        while (!hits.empty() &&
               std::chrono::duration_cast<std::chrono::milliseconds>(now - hits.front()).count() > DENSE_NOTE_WINDOW_MS)
            hits.pop_front();
        bool currDense = hits.size() >= (size_t)DENSE_NOTE_THRESHOLD;
        if (currDense != isDenseMode) {
            isDenseMode = currDense;
            std::cout << "[DENSE] " << (isDenseMode ? "ON\n" : "OFF\n");
        }

        // --- Xử lý từng lane ---
        for (auto& [ch, nk] : keyMap) {
            // 1. Predictive scan
            POINT predictPt = { nk.relativePos.x,
                                PREDICT_SCAN_REGION_Y_RELATIVE - SCAN_REGION_Y };
            nk.isNoteIncoming = isColorPresentInArea(
                hdcMem, predictPt,
                NOTE_HEAD_SEARCH_BOX_SIZE, NOTE_COLOR, COLOR_TOLERANCE);

            nk.isIncomingNoteHoldCandidate = nk.isNoteIncoming &&
                isHoldBodyPresent(
                  hdcMem, predictPt,
                  HOLD_BODY_SEARCH_LENGTH + (SCAN_REGION_Y - PREDICT_SCAN_REGION_Y_RELATIVE),
                  HOLD_BODY_SEARCH_RADIUS);

            // 2. Main reaction at judgement line
            bool headPresent = isColorPresentInArea(
                hdcMem, nk.relativePos,
                NOTE_HEAD_SEARCH_BOX_SIZE, NOTE_COLOR, COLOR_TOLERANCE);

            bool holdNow = isHoldBodyPresent(
                hdcMem, nk.relativePos,
                HOLD_BODY_SEARCH_LENGTH, HOLD_BODY_SEARCH_RADIUS);

            bool actualHold = holdNow || nk.isIncomingNoteHoldCandidate;

            // --- Nhấn phím ---
            if (!nk.isHolding && headPresent) {
                auto sinceRel = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - nk.lastReleaseTime).count();
                if (sinceRel >= MIN_TIME_BETWEEN_KEY_PRESSES_MS) {
                    press(nk.virtualKey);
                    nk.lastPressTime = now;
                    hits.push_back(now);

                    if (actualHold) {
                        nk.isHolding = true;
                        nk.holdStartTime = now;
                        nk.lastHoldBodyDetectedTime = now;
                    } else {
                        release(nk.virtualKey);
                        nk.lastReleaseTime = now;
                    }
                }
            }

            // --- Nhả phím nếu đang giữ ---
            if (nk.isHolding) {
                if (holdNow)
                    nk.lastHoldBodyDetectedTime = now;

                auto sinceBody = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - nk.lastHoldBodyDetectedTime).count();

                if (!headPresent && !holdNow && sinceBody > HOLD_RELEASE_DEBOUNCE_MS) {
                    release(nk.virtualKey);
                    nk.isHolding = false;
                    nk.lastReleaseTime = now;
                }
            }
        }

        // Nếu bạn cần sleep chút để giảm CPU, bỏ comment dòng dưới:
        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}