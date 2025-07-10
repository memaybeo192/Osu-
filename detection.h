#pragma once
#include <Windows.h>
#include "Config.h"

inline COLORREF GetColorFromPixelBuffer(BYTE* pPixels, int bufferWidth, int bufferHeight, int x, int y) {
    if (x < 0 || x >= bufferWidth || y < 0 || y >= bufferHeight) {
        return CLR_INVALID;
    }
    long index = (y * bufferWidth + x) * 4;
    BYTE b = pPixels[index];
    BYTE g = pPixels[index + 1];
    BYTE r = pPixels[index + 2];
    return RGB(r, g, b);
}

inline bool isColorNear(COLORREF a, COLORREF b, int tol) {
    return abs(GetRValue(a) - GetRValue(b)) <= tol &&
           abs(GetGValue(a) - GetGValue(b)) <= tol &&
           abs(GetBValue(a) - GetBValue(b)) <= tol;
}

bool isHoldBodyPresent(BYTE* pPixels, int bufferWidth, int bufferHeight, POINT topPos, int length, int radius) {
    for (int y = -HOLD_BODY_INITIAL_UPWARD_SCAN_OFFSET; y < length; y += 2) {
        int yy = topPos.y + y;
        if (yy < 0 || yy >= bufferHeight) continue;

        for (int dx = -radius; dx <= radius; dx++) {
            int xx = topPos.x + dx;
            if (xx < 0 || xx >= bufferWidth) continue;
            
            COLORREF c = GetColorFromPixelBuffer(pPixels, bufferWidth, bufferHeight, xx, yy);
            if (c != CLR_INVALID && isColorNear(c, HOLD_BODY_COLOR, HOLD_TOLERANCE))
                return true;
        }
    }
    return false;
}

bool isColorPresentInArea(BYTE* pPixels, int bufferWidth, int bufferHeight, POINT center, int boxSize, COLORREF target, int tol) {
    for (int dx = -boxSize; dx <= boxSize; dx++) {
        for (int dy = -boxSize; dy <= boxSize; dy++) {
            int x = center.x + dx;
            int y = center.y + dy;
            if (x < 0 || x >= bufferWidth || y < 0 || y >= bufferHeight)
                continue;
            COLORREF c = GetColorFromPixelBuffer(pPixels, bufferWidth, bufferHeight, x, y);
            if (c != CLR_INVALID && isColorNear(c, target, tol))
                return true;
        }
    }
    return false;
}
