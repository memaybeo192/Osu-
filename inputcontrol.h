#pragma once
#include <Windows.h>

// Nhấn phím
inline void press(int vk) {
    keybd_event((BYTE)vk, 0, 0, 0);
}

// Nhả phím
inline void release(int vk) {
    keybd_event((BYTE)vk, 0, KEYEVENTF_KEYUP, 0);
}