#pragma once
#include <Windows.h>
#include "Config.h"

// So sánh 2 màu có gần nhau không
inline bool isColorNear(COLORREF a, COLORREF b, int tol) {
    return abs(GetRValue(a) - GetRValue(b)) <= tol &&
           abs(GetGValue(a) - GetGValue(b)) <= tol &&
           abs(GetBValue(a) - GetBValue(b)) <= tol;
}

// Quét thân note giữ từ y = topPos.y - offset  xuống topPos.y + length
bool isHoldBodyPresent(HDC hdc, POINT topPos, int length, int radius) {
    for (int y = -HOLD_BODY_INITIAL_UPWARD_SCAN_OFFSET; y < length; y += 3) {
        int yy = topPos.y + y;
        if (yy < 0 || yy >= SCAN_REGION_HEIGHT) continue;
        for (int dx = -radius; dx <= radius; dx++) {
            int xx = topPos.x + dx;
            if (xx < 0 || xx >= SCAN_REGION_WIDTH) continue;
            COLORREF c = GetPixel(hdc, xx, yy);
            if (c != CLR_INVALID && isColorNear(c, HOLD_BODY_COLOR, HOLD_TOLERANCE))
                return true;
        }
    }
    return false;
}

// Quét tap note quanh điểm center
bool isColorPresentInArea(HDC hdc, POINT center, int boxSize, COLORREF target, int tol) {
    for (int dx = -boxSize; dx <= boxSize; dx++) {
        for (int dy = -boxSize; dy <= boxSize; dy++) {
            int x = center.x + dx;
            int y = center.y + dy;
            if (x < 0 || x >= SCAN_REGION_WIDTH || y < 0 || y >= SCAN_REGION_HEIGHT)
                continue;
            COLORREF c = GetPixel(hdc, x, y);
            if (c != CLR_INVALID && isColorNear(c, target, tol))
                return true;
        }
    }
    return false;
}