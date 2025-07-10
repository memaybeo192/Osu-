#pragma once
#include <Windows.h>
#include <chrono>
#include <map>

struct NoteKey {
    POINT relativePos;    // Tọa độ lane (tính từ góc trên-trái của scan region)
    int virtualKey;       // VK_S, VK_D, ...
    bool isHolding = false;

    std::chrono::steady_clock::time_point lastPressTime;
    std::chrono::steady_clock::time_point lastReleaseTime;
    std::chrono::steady_clock::time_point holdStartTime;
    std::chrono::steady_clock::time_point lastHoldBodyDetectedTime;

    bool isNoteIncoming = false;
    bool isIncomingNoteHoldCandidate = false;

    NoteKey(POINT pos, int vk)
      : relativePos(pos), virtualKey(vk)
    {
        auto now = std::chrono::steady_clock::now();
        lastPressTime = lastReleaseTime = holdStartTime = lastHoldBodyDetectedTime = now;
    }
};

// Khởi tạo 4 lane S D J K
static std::map<char, NoteKey> keyMap = {
    { 'S', NoteKey{{ 88, 268 }, 0x53} },
    { 'D', NoteKey{{240, 268 }, 0x44} },
    { 'J', NoteKey{{382, 268 }, 0x4A} },
    { 'K', NoteKey{{531, 268 }, 0x4B} }
};