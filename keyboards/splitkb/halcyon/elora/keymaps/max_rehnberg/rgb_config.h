// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"
#include "rgb_matrix.h"
#include "color.h"

#define SPLIT_TRANSPORT_MIRROR
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_MODS_ENABLE
#ifndef RGBLIGHT_LIMIT_VAL
#    define RGBLIGHT_LIMIT_VAL 150
#endif

// LED Index Definitions
#define LED_ARROW_UP 27
#define LED_ARROW_DOWN 21
#define LED_ARROW_LEFT 22
#define LED_ARROW_RIGHT 20

// Bottom row
#define LED_THUMB_KEY_0 6
#define LED_THUMB_KEY_1 7
#define LED_THUMB_KEY_2 8
#define LED_THUMB_KEY_3 9
#define LED_THUMB_KEY_4 10

// upper row
#define LED_THUMB_KEY_5 11
#define LED_THUMB_KEY_6 12

#define LED_LAYER_NAV LED_THUMB_KEY_2
#define LED_LAYER_ADJUST LED_THUMB_KEY_4
#define LED_CAPS_WORD 18
#define LED_CW_UNDERGLOW 3
#define LED_ONESHOT_SHIFT_UNDERGLOW 5
#define LED_ADJUST 10

#define LED_LEADER 12
#define LED_LEADER_UNDERGLOW 5

// RGB configuration types
typedef enum {
    RGB_TYPE_ARRAY,  // Array of LED indices
    RGB_TYPE_SINGLE, // Single LED
    RGB_TYPE_ALL     // All LEDs
} rgb_config_type_t;

// Side flags for split keyboard RGB configurations
typedef enum {
    RGB_SIDE_BOTH, // Apply to both sides
    RGB_SIDE_LEFT, // Apply only to left side (master)
    RGB_SIDE_RIGHT // Apply only to right side (slave)
} rgb_side_t;

// Key group configuration structures
typedef struct {
    uint8_t           hue;
    uint8_t           saturation;
    uint8_t           value;
    rgb_config_type_t type;
    union {
        struct {
            const uint8_t *led_indices;
            uint8_t        led_count;
        } array;
        struct {
            uint8_t led_index;
        } single;
        // RGB_TYPE_ALL needs no additional data
    } data;
    rgb_side_t side; // Which side this config applies to (moved to end for compatibility)
} led_rgb_config_t;

// Helper macros for cleaner configuration
// These work by breaking down the nested designated initializers to avoid C parser issues
#define MAKE_RGB_ARRAY_DATA(array_ptr, array_count) .array = {.led_indices = (array_ptr), .led_count = (array_count)}
#define MAKE_RGB_SINGLE_DATA(index) .single = {.led_index = (index)}

#define RGB_CONFIG_ARRAY(h, s, v, array, count, side_value) {.hue = (h), .saturation = (s), .value = (v), .type = RGB_TYPE_ARRAY, .data = {MAKE_RGB_ARRAY_DATA(array, count)}, .side = (side_value)}

#define RGB_CONFIG_SINGLE(h, s, v, led_index, side_value) {.hue = (h), .saturation = (s), .value = (v), .type = RGB_TYPE_SINGLE, .data = {MAKE_RGB_SINGLE_DATA(led_index)}, .side = (side_value)}

#define RGB_CONFIG_ALL(h, s, v) {.hue = (h), .saturation = (s), .value = (v), .type = RGB_TYPE_ALL}

typedef struct {
    const led_rgb_config_t *configs;
    uint8_t                 config_count;
} layer_rgb_config_t;

// Function declarations
void set_hsv_by_key_indices(const uint8_t *indices, uint8_t array_size, hsv_t hsv);
void set_layer_rgb_by_configs(const layer_rgb_config_t *config);
void set_caps_word_rgb(hsv_t hsv, uint8_t led_min, uint8_t led_max);
bool is_current_side_master(void);

// External configuration declarations
extern const layer_rgb_config_t mouse_layer_config;
extern const layer_rgb_config_t nav_layer_config;
extern const layer_rgb_config_t adjust_layer_config;
extern const layer_rgb_config_t symbol_layer_config;
extern const layer_rgb_config_t base_layer_config;
extern const layer_rgb_config_t bootloader_config;
extern const layer_rgb_config_t caps_word_layer_config;
extern const layer_rgb_config_t leader_layer_config;
extern const layer_rgb_config_t sentence_case_layer_config;
extern const layer_rgb_config_t sentence_case_primed_layer_config;
extern const layer_rgb_config_t oneshot_shift_layer_config;
extern const layer_rgb_config_t num_layer_config;
