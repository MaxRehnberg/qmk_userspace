// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "rgb_config.h"
#include "color.h"

// Helper function to check if a config should be applied to current side
static bool should_apply_config(rgb_side_t config_side) {
    bool is_master = is_keyboard_left();

    switch (config_side) {
        case RGB_SIDE_BOTH:
            return true;
        case RGB_SIDE_LEFT:
            return is_master;
        case RGB_SIDE_RIGHT:
            return !is_master;
        default:
            return true;
    }
}

// Shared LED Groups

static const uint8_t arrow_keys[] = {LED_ARROW_UP, LED_ARROW_DOWN, LED_ARROW_LEFT, LED_ARROW_RIGHT};
static const uint8_t num_pad[]    = {8, 14, 15, 16, 20, 21, 22, 26, 27, 28};
static const uint8_t backplate[]  = {0, 1, 2, 3, 4, 5};

// Layer Configurations
static const uint8_t mouse_accel_keys[] = {20, 21, 22};

const layer_rgb_config_t num_layer_config   = {(const led_rgb_config_t[]){RGB_CONFIG_ARRAY(85, 255, 100, num_pad, 10, RGB_SIDE_RIGHT), // Green numpad keys
                                                                          RGB_CONFIG_SINGLE(85, 255, 100, 7, RGB_SIDE_LEFT)},
                                               2};
const layer_rgb_config_t mouse_layer_config = {(const led_rgb_config_t[]){
                                                   RGB_CONFIG_ARRAY(43, 255, 175, arrow_keys, 4, RGB_SIDE_LEFT),        // Yellow arrow keys
                                                   RGB_CONFIG_ARRAY(85, 255, 175, mouse_accel_keys, 3, RGB_SIDE_RIGHT), // Green acceleration keys
                                                   RGB_CONFIG_SINGLE(43, 255, 175, LED_THUMB_KEY_3, RGB_SIDE_RIGHT),    // Yellow layer indicator
                                                   RGB_CONFIG_ARRAY(43, 255, 100, backplate, 6, RGB_SIDE_BOTH),         // Yellow backplate
                                               },
                                               4};

const layer_rgb_config_t nav_layer_config = {(const led_rgb_config_t[]){
                                                 RGB_CONFIG_ARRAY(15, 255, 150, arrow_keys, 4, RGB_SIDE_LEFT),   // Orange arrow keys
                                                 RGB_CONFIG_SINGLE(15, 255, 150, LED_LAYER_NAV, RGB_SIDE_RIGHT), // Orange layer key (single LED)
                                                 RGB_CONFIG_ARRAY(15, 255, 100, backplate, 6, RGB_SIDE_BOTH),    // Orange backplate
                                             },
                                             3};

static const uint8_t alpha_keys[] = {13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 25, 26, 27, 28, 29};
static const uint8_t sym_key      = 7;

static const led_rgb_config_t symbol_layer_configs[] = {
    RGB_CONFIG_ARRAY(200, 255, 150, alpha_keys, 15, RGB_SIDE_BOTH), // purple symbol keys
    RGB_CONFIG_SINGLE(200, 255, 150, sym_key, RGB_SIDE_RIGHT),      // purple symbol key
    RGB_CONFIG_ARRAY(200, 255, 100, backplate, 6, RGB_SIDE_BOTH)    // purple backplate
};

const layer_rgb_config_t symbol_layer_config = {symbol_layer_configs, 3};

// Adjust layer configuration
static const uint8_t     base_layer_keys[]   = {15, 21, 27};
const layer_rgb_config_t adjust_layer_config = {(const led_rgb_config_t[]){
                                                    RGB_CONFIG_SINGLE(0, 255, 100, LED_ADJUST, RGB_SIDE_LEFT),        // Red adjust key
                                                    RGB_CONFIG_ARRAY(85, 255, 175, base_layer_keys, 3, RGB_SIDE_LEFT) // Green base layers
                                                },
                                                2};

const layer_rgb_config_t caps_word_layer_config = {(const led_rgb_config_t[]){
                                                       RGB_CONFIG_SINGLE(20, 255, 100, LED_CAPS_WORD, RGB_SIDE_LEFT),   // Red caps word key
                                                       RGB_CONFIG_SINGLE(20, 255, 100, LED_CW_UNDERGLOW, RGB_SIDE_LEFT) // Red underglow LED
                                                   },
                                                   2};

const layer_rgb_config_t oneshot_shift_layer_config = {(const led_rgb_config_t[]){RGB_CONFIG_SINGLE(20, 255, 100, LED_ONESHOT_SHIFT_UNDERGLOW, RGB_SIDE_BOTH), // Red underglow for oneshot shift
                                                                                  RGB_CONFIG_SINGLE(20, 255, 50, 11, RGB_SIDE_BOTH)},
                                                       2};

const layer_rgb_config_t leader_layer_config = {(const led_rgb_config_t[]){RGB_CONFIG_SINGLE(20, 255, 100, LED_LEADER, RGB_SIDE_RIGHT), RGB_CONFIG_SINGLE(20, 255, 50, LED_LEADER_UNDERGLOW, RGB_SIDE_RIGHT)}, 2};

const layer_rgb_config_t sentence_case_layer_config = {(const led_rgb_config_t[]){RGB_CONFIG_SINGLE(20, 255, 100, LED_ADJUST, RGB_SIDE_RIGHT), RGB_CONFIG_SINGLE(20, 255, 50, LED_CW_UNDERGLOW, RGB_SIDE_RIGHT)}, 2};

const layer_rgb_config_t sentence_case_primed_layer_config = {(const led_rgb_config_t[]){RGB_CONFIG_SINGLE(20, 255, 150, LED_ADJUST, RGB_SIDE_RIGHT), RGB_CONFIG_SINGLE(20, 255, 100, LED_CW_UNDERGLOW, RGB_SIDE_RIGHT)}, 2};

// Single-config layers for special scenarios
const layer_rgb_config_t base_layer_config = {(const led_rgb_config_t[]){
                                                  RGB_CONFIG_ALL(0, 0, 100) // Low white for base layer
                                              },
                                              1};

const layer_rgb_config_t bootloader_config = {(const led_rgb_config_t[]){RGB_CONFIG_ALL(0, 255, 100)}, 1};

// RGB utility functions
void set_hsv_by_key_indices(const uint8_t *indices, uint8_t array_size, hsv_t hsv) {
    rgb_t color = hsv_to_rgb(hsv);
    for (uint8_t i = 0; i < array_size; i++) {
        rgb_matrix_set_color(indices[i], color.r, color.g, color.b);
    }
}

// Generic function to apply multiple RGB configs to a layer
void set_layer_rgb_by_configs(const layer_rgb_config_t *config) {
    if (config == NULL || config->configs == NULL) {
        return;
    }

    for (uint8_t i = 0; i < config->config_count; i++) {
        const led_rgb_config_t *current_config = &config->configs[i];

        // For RGB_TYPE_ARRAY and RGB_TYPE_SINGLE, check the side field. For RGB_TYPE_ALL, assume RGB_SIDE_BOTH
        rgb_side_t config_side = (current_config->type == RGB_TYPE_ALL) ? RGB_SIDE_BOTH : current_config->side;

        // Check if this config should be applied to the current side
        if (!should_apply_config(config_side)) {
            continue;
        }

        hsv_t hsv   = {current_config->hue, current_config->saturation, current_config->value};
        rgb_t color = hsv_to_rgb(hsv);

        switch (current_config->type) {
            case RGB_TYPE_SINGLE:
                // Handle single LED
                rgb_matrix_set_color(current_config->data.single.led_index, color.r, color.g, color.b);
                break;
            case RGB_TYPE_ARRAY:
                // Handle array of LEDs
                set_hsv_by_key_indices(current_config->data.array.led_indices, current_config->data.array.led_count, hsv);
                break;
            case RGB_TYPE_ALL:
                // Handle all LEDs
                rgb_matrix_set_color_all(color.r, color.g, color.b);
                break;
        }
    }
}
