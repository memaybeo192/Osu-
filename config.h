#pragma once
#include <Windows.h>

#define NOTE_COLOR          RGB(255, 255, 255)
#define HOLD_BODY_COLOR     RGB(118, 117, 120)
#define BACKGROUND_COLOR    RGB(0, 0, 0)

static const int COLOR_TOLERANCE              = 40;
static const int HOLD_TOLERANCE               = 45;

// Các thông số vùng quét màn hình (đã được đo từ ảnh của bạn)
static const int SCAN_REGION_X                = 648; 
static const int SCAN_REGION_Y                = 121; 
static const int SCAN_REGION_WIDTH            = 624; 
static const int SCAN_REGION_HEIGHT           = 859; 

static const int NOTE_HEAD_SEARCH_BOX_SIZE    = 14;
static const int HOLD_BODY_SEARCH_LENGTH      = 350;
static const int HOLD_BODY_SEARCH_RADIUS      = 8;
static const int HOLD_BODY_INITIAL_UPWARD_SCAN_OFFSET = 5;

static const int MIN_TIME_BETWEEN_KEY_PRESSES_MS = 0;
static const int PREDICT_SCAN_REGION_Y_RELATIVE  = 49;

// Offset cho phép người dùng tinh chỉnh thời điểm bấm phím
// Một giá trị dương sẽ làm bot bấm TRỄ hơn (di chuyển điểm quét xuống dưới)
// Một giá trị âm sẽ làm bot bấm SỚM hơn (di chuyển điểm quét lên trên)
static const int OFFSET_Y = 10; // Mặc định là 0. Bạn có thể thay đổi giá trị này!

static const int DENSE_NOTE_WINDOW_MS           = 500;
static const int DENSE_NOTE_THRESHOLD           = 20;

static const int TOGGLE_HOTKEY               = VK_PRIOR; // Phím tắt để bật/tắt bot (PgUp)
static const wchar_t* OSU_WINDOW_TITLE       = L"osu!";
