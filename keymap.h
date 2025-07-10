#pragma once
#include <Windows.h>
#include <chrono>
#include <map>

struct NoteKey {
    POINT relativePos;
    int virtualKey;
    bool isHolding = false;

    std::chrono::steady_clock::time_point lastPressTime;
    std::chrono::steady_clock::time_point lastReleaseTime;
    std::chrono::steady_clock::time_point holdStartTime;
    std::chrono::steady_clock::time_point lastHoldBodyDetectedTime;

    bool isNoteIncoming = false;
    bool isIncomingNoteHoldCandidate = false;

    int holdFrameLossCount = 0;
    static constexpr int MAX_HOLD_FRAME_LOSS = 2; // Giữ 2 để xem nốt hold có nhả sớm không.

    NoteKey(POINT pos, int vk)
      : relativePos(pos), virtualKey(vk)
    {
        auto now = std::chrono::steady_clock::now();
        lastPressTime = lastReleaseTime = holdStartTime = lastHoldBodyDetectedTime = now;
    }
};

static std::map<char, NoteKey> keyMap = {
    // Giá trị Y này là BASE_Y, OFFSET_Y sẽ được cộng vào trong botstate.h
    { 'S', NoteKey{{ 79, 830 }, 0x53} },
    { 'D', NoteKey{{235, 830 }, 0x44} },
    { 'J', NoteKey{{380, 830 }, 0x4A} },
    { 'K', NoteKey{{535, 830 }, 0x4B} }
};
