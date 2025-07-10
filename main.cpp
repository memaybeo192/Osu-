#include <Windows.h>
#include <iostream>
#include <vector>
#include <chrono>
#include "BotState.h"
#include "Config.h" // Cần thiết cho các định nghĩa SCAN_REGION_X/Y/WIDTH/HEIGHT

// Khai báo hàm botLoop (được định nghĩa trong BotState.h)
void botLoop(HDC hdcScreen, HDC hdcMem, HBITMAP hbmScreen, BYTE* pPixels, BITMAPINFOHEADER& bi);

int main() {
    timeBeginPeriod(1);

    std::cout << "[AUTO Osu! Mania Bot – Optimized]\n";
    std::cout << "Scan region: X=" << SCAN_REGION_X
              << " Y=" << SCAN_REGION_Y
              << " W=" << SCAN_REGION_WIDTH
              << " H=" << SCAN_REGION_HEIGHT << "\n";
    std::cout << "Press PgUp to toggle bot.\n\n";

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem    = CreateCompatibleDC(hdcScreen);
    HBITMAP hbm   = CreateCompatibleBitmap(hdcScreen, SCAN_REGION_WIDTH, SCAN_REGION_HEIGHT);
    SelectObject(hdcMem, hbm);

    if (!hdcScreen || !hdcMem || !hbm) {
        std::cerr << "[ERROR] Failed to initialize screen capture devices.\n";
        return 1;
    }

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = SCAN_REGION_WIDTH;
    bi.biHeight = -SCAN_REGION_HEIGHT;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    std::vector<BYTE> pixelBuffer(SCAN_REGION_WIDTH * SCAN_REGION_HEIGHT * 4);
    BYTE* pPixels = pixelBuffer.data();

    botLoop(hdcScreen, hdcMem, hbm, pPixels, bi);

    DeleteObject(hbm);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    timeEndPeriod(1);
    return 0;
}
