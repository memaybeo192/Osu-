#pragma once
#include <Windows.h>

// ------------------ COLORS & TOLERANCES ------------------
#define NOTE_COLOR          RGB(255, 255, 255)   // Tap note
#define HOLD_BODY_COLOR     RGB(118, 117, 120)   // Hold note body
#define BACKGROUND_COLOR    RGB(0,   0,   0)     // Background

static const int COLOR_TOLERANCE              = 40;
static const int HOLD_TOLERANCE               = 45;

// ------------------ SCREEN REGION ------------------
static const int SCAN_REGION_X                = 650;
static const int SCAN_REGION_Y                = 705;
static const int SCAN_REGION_WIDTH            = 600;
static const int SCAN_REGION_HEIGHT           = 300;

// ------------------ NOTE DETECTION ------------------
static const int NOTE_HEAD_SEARCH_BOX_SIZE    = 14;
static const int HOLD_BODY_SEARCH_LENGTH      = 300;
static const int HOLD_BODY_SEARCH_RADIUS      = 6;
static const int HOLD_BODY_INITIAL_UPWARD_SCAN_OFFSET = 5;
static const int HOLD_RELEASE_DEBOUNCE_MS     = 25;

// ------------------ TIMING & PREDICTION ------------------
static const int MIN_TIME_BETWEEN_KEY_PRESSES_MS = 0;
static const int PREDICT_SCAN_REGION_Y_RELATIVE  = SCAN_REGION_Y - 150;
static const int PREDICT_SCAN_REGION_HEIGHT      = 7;

// ------------------ DENSE MODE ------------------
static const int DENSE_NOTE_WINDOW_MS           = 500;
static const int DENSE_NOTE_THRESHOLD           = 15;

// ------------------ HOTKEY & WINDOW ------------------
static const int TOGGLE_HOTKEY               = VK_PRIOR;     // PgUp
static const wchar_t* OSU_WINDOW_TITLE       = L"osu!";

// ------------------ STARTUP WAIT ------------------
static const int REQUIRED_CLEAR_GAME_AREA_MS = 700;
static const int MAX_ACTIVE_WAIT_SECONDS     = 1;

// ------------------ SCAN INTERVAL ------------------
static const int CHECK_INTERVAL_MS           = 0;            // 0 = no explicit sleep