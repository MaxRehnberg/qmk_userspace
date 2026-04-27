// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "quantum.h"

// LED Index Definitions
#define LED_ARROW_UP 27
#define LED_ARROW_DOWN 21
#define LED_ARROW_LEFT 22
#define LED_ARROW_RIGHT 20

enum {
    LED_THUMB_KEY_0 = 6,
    LED_THUMB_KEY_1,
    LED_THUMB_KEY_2,
    LED_THUMB_KEY_3,
    LED_THUMB_KEY_4,
    LED_THUMB_KEY_5,
    LED_THUMB_KEY_6,
};

#define LED_LAYER_NAV LED_THUMB_KEY_2
#define LED_CAPS_WORD 18
#define LED_CW_UNDERGLOW 3
#define LED_ONESHOT_SHIFT_UNDERGLOW 5
#define LED_ADJUST 10

#define LED_LEADER 12
#define LED_LEADER_UNDERGLOW 5

// Elora split RGB mapping uses global indices (left: 0-36, right: 37-73)
#define ELORA_RIGHT_LED_OFFSET 37
#define RGB_RIGHT_LED(index) ((uint8_t)((index) + ELORA_RIGHT_LED_OFFSET))

// RGB configuration types
typedef enum {
    RGB_TYPE_ARRAY,  // Array of LED indices
    RGB_TYPE_SINGLE, // Single LED
    RGB_TYPE_ALL     // All LEDs
} rgb_config_type_t;

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
} led_rgb_config_t;

// Helper macros for cleaner configuration
// These work by breaking down the nested designated initializers to avoid C parser issues
#define MAKE_RGB_ARRAY_DATA(array_ptr, array_count) .array = {.led_indices = (array_ptr), .led_count = (array_count)}
#define MAKE_RGB_SINGLE_DATA(index) .single = {.led_index = (index)}

#define RGB_CONFIG_ARRAY(h, s, v, array, count)                                                                            \
    {                                                                                                                      \
        .hue = (h), .saturation = (s), .value = (v), .type = RGB_TYPE_ARRAY, .data = { MAKE_RGB_ARRAY_DATA(array, count) } \
    }

#define RGB_CONFIG_SINGLE(h, s, v, led_index)                                                                             \
    {                                                                                                                     \
        .hue = (h), .saturation = (s), .value = (v), .type = RGB_TYPE_SINGLE, .data = { MAKE_RGB_SINGLE_DATA(led_index) } \
    }

#define RGB_CONFIG_ALL(h, s, v) {.hue = (h), .saturation = (s), .value = (v), .type = RGB_TYPE_ALL}

typedef struct {
    const led_rgb_config_t *configs;
    uint8_t                 config_count;
} layer_rgb_config_t;

// Function declarations
void set_layer_rgb_by_configs(const layer_rgb_config_t *config);

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
