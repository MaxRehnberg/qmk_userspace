#pragma once
#define SELECT_WORD_OS_MAC

#define MK_3_SPEED 1
#define MK_C_OFFSET_0 4
#define MK_C_OFFSET_1 16
#define MK_C_OFFSET_2 32

#ifndef RGBLIGHT_LIMIT_VAL
#define RGBLIGHT_LIMIT_VAL 150
#endif

#define SPLIT_LAYER_STATE_ENABLE
#define RGB_TRIGGER_ON_KEYDOWN

// Home row definitions
#define DUMMY_MOD_NEUTRALIZER_KEYCODE KC_F18
#define TAPPING_TERM 250
#define CHORDAL_HOLD
#define PERMISSIVE_HOLD 
#define FLOW_TAP_TERM 150

#define DEBOUNCE 15

// Timing constants
#define APP_SWITCHER_TIMEOUT 1000 // 1 second

#define QUICK_TAP_TERM 0