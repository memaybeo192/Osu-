#pragma once

#include <Windows.h>
#include <iostream>
#include <deque>
#include <thread>
#include <chrono>
#include <map>
#include <vector>

#include "Config.h"
#include "KeyMap.h"
#include "Detection.h"
#include "InputControl.h"

bool isWindowActive(const wchar_t* title) {
    HWND hwnd = GetForegroundWindow();
    wchar_t buff[256];
    GetWindowTextW(hwnd, buff, 256);
    return wcsstr(buff, title) != nullptr;
}

void resetKey(NoteKey& nk, std::chrono::steady_clock::time_point now) {
    if (nk.isHolding) {
        release(nk.virtualKey);
    }
    nk.isHolding = false;
    nk.holdFrameLossCount = 0;
    nk.isNoteIncoming = false;
    nk.isIncomingNoteHoldCandidate = false;
    nk.lastPressTime = now;
    nk.lastReleaseTime = now;
    nk.holdStartTime = now;
    nk.lastHoldBodyDetectedTime = now;
}

void botLoop(HDC hdcScreen, HDC hdcMem, HBITMAP hbmScreen, BYTE* pPixels, BITMAPINFOHEADER& bi) {
    std::deque<std::chrono::steady_clock::time_point> hits;
    bool botActive = false;
    bool isDenseMode = false;
    bool prevBotActive = false;

    std::cout << "[INFO] Bot INACTIVE. Nhấn PgUp để toggle.\n";

    while (true) {
        auto now = std::chrono::steady_clock::now();

        if (GetAsyncKeyState(TOGGLE_HOTKEY) & 0x8000) {
            botActive = !botActive;
            std::cout << "[INFO] Bot " << (botActive ? "ON\n" : "OFF\n");

            for (auto& pair : keyMap) {
                resetKey(pair.second, now);
            }
            hits.clear();
            isDenseMode = false;

            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        if (!botActive) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        if (!isWindowActive(OSU_WINDOW_TITLE)) {
            for (auto& pair : keyMap) {
                resetKey(pair.second, now);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            prevBotActive = false;
            continue;
        }

        if (!prevBotActive && botActive) {
             std::cout << "[INFO] Resetting bot state due to activation or window focus.\n";
            for (auto& pair : keyMap) {
                resetKey(pair.second, now);
            }
            hits.clear();
            isDenseMode = false;
        }
        prevBotActive = botActive;

        BitBlt(hdcMem, 0, 0, SCAN_REGION_WIDTH, SCAN_REGION_HEIGHT,
               hdcScreen, SCAN_REGION_X, SCAN_REGION_Y, SRCCOPY);
        
        GetDIBits(hdcMem, hbmScreen, 0, SCAN_REGION_HEIGHT, pPixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

        while (!hits.empty() &&
               std::chrono::duration_cast<std::chrono::milliseconds>(now - hits.front()).count() > DENSE_NOTE_WINDOW_MS) {
            hits.pop_front();
        }
        bool currDense = hits.size() >= DENSE_NOTE_THRESHOLD;
        if (currDense != isDenseMode) {
            isDenseMode = currDense;
            std::cout << "[DENSE] " << (isDenseMode ? "ON\n" : "OFF\n");
        }

        for (auto& pair : keyMap) {
            NoteKey& nk = pair.second;

            // Áp dụng OFFSET_Y vào vị trí dự đoán
            POINT predictPt = {
                nk.relativePos.x,
                PREDICT_SCAN_REGION_Y_RELATIVE + OFFSET_Y // Áp dụng OFFSET_Y
            };
            nk.isNoteIncoming = isColorPresentInArea(pPixels, SCAN_REGION_WIDTH, SCAN_REGION_HEIGHT, predictPt,
                NOTE_HEAD_SEARCH_BOX_SIZE, NOTE_COLOR, COLOR_TOLERANCE);

            nk.isIncomingNoteHoldCandidate = nk.isNoteIncoming &&
                isHoldBodyPresent(pPixels, SCAN_REGION_WIDTH, SCAN_REGION_HEIGHT, predictPt,
                    HOLD_BODY_SEARCH_LENGTH + (nk.relativePos.y - PREDICT_SCAN_REGION_Y_RELATIVE),
                    HOLD_BODY_SEARCH_RADIUS);

            // Áp dụng OFFSET_Y vào vị trí chấm điểm (headPresent)
            POINT currentScanPt = {
                nk.relativePos.x,
                nk.relativePos.y + OFFSET_Y // Áp dụng OFFSET_Y
            };
            bool headPresent = isColorPresentInArea(pPixels, SCAN_REGION_WIDTH, SCAN_REGION_HEIGHT, currentScanPt,
                NOTE_HEAD_SEARCH_BOX_SIZE, NOTE_COLOR, COLOR_TOLERANCE);

            bool holdNow = isHoldBodyPresent(pPixels, SCAN_REGION_WIDTH, SCAN_REGION_HEIGHT, currentScanPt,
                HOLD_BODY_SEARCH_LENGTH, HOLD_BODY_SEARCH_RADIUS);

            bool actualHold = holdNow || nk.isIncomingNoteHoldCandidate;

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
                        nk.holdFrameLossCount = 0;
                    } else {
                        release(nk.virtualKey);
                        nk.lastReleaseTime = now;
                    }
                }
            }

            if (nk.isHolding) {
                bool holdBodyStillThere = isHoldBodyPresent(pPixels, SCAN_REGION_WIDTH, SCAN_REGION_HEIGHT, currentScanPt,
                    HOLD_BODY_SEARCH_LENGTH, HOLD_BODY_SEARCH_RADIUS);

                if (holdBodyStillThere) {
                    nk.lastHoldBodyDetectedTime = now;
                    nk.holdFrameLossCount = 0;
                } else {
                    nk.holdFrameLossCount++;
                }

                if (!headPresent && nk.holdFrameLossCount >= NoteKey::MAX_HOLD_FRAME_LOSS) {
                    release(nk.virtualKey);
                    nk.isHolding = false;
                    nk.lastReleaseTime = now;
                }
            }
        }
    }
}
