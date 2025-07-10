#include <Windows.h>
#include <iostream>
#include "BotState.h"

int main() {
    // 1ms timer precision
    timeBeginPeriod(1);

    std::cout << "[AUTO Osu! Mania Bot – ReStructured]\n";
    std::cout << "Scan region: X=" << SCAN_REGION_X
              << " Y=" << SCAN_REGION_Y
              << " W=" << SCAN_REGION_WIDTH
              << " H=" << SCAN_REGION_HEIGHT << "\n";
    std::cout << "Hold debounce: " << HOLD_RELEASE_DEBOUNCE_MS << "ms\n";
    std::cout << "Press PgUp to toggle bot.\n\n";

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem    = CreateCompatibleDC(hdcScreen);
    HBITMAP hbm   = CreateCompatibleBitmap(hdcScreen, SCAN_REGION_WIDTH, SCAN_REGION_HEIGHT);
    SelectObject(hdcMem, hbm);

    if (!hdcScreen || !hdcMem || !hbm) {
        std::cerr << "[ERROR] Failed to init screen capture.\n";
        return 1;
    }

    botLoop(hdcScreen, hdcMem, hbm);

    // Cleanup
    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    timeEndPeriod(1);
    return 0;
}