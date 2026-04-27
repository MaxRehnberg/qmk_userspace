// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "rgb_config.h"
#include "color.h"

// Shared LED Groups

static const uint8_t arrow_keys_left[]        = {LED_ARROW_UP, LED_ARROW_DOWN, LED_ARROW_LEFT, LED_ARROW_RIGHT};
static const uint8_t num_pad_right[]          = {RGB_RIGHT_LED(8), RGB_RIGHT_LED(14), RGB_RIGHT_LED(15), RGB_RIGHT_LED(16), RGB_RIGHT_LED(20), RGB_RIGHT_LED(21), RGB_RIGHT_LED(22), RGB_RIGHT_LED(26), RGB_RIGHT_LED(27), RGB_RIGHT_LED(28)};
static const uint8_t backplate_both[]         = {0, 1, 2, 3, 4, 5, RGB_RIGHT_LED(0), RGB_RIGHT_LED(1), RGB_RIGHT_LED(2), RGB_RIGHT_LED(3), RGB_RIGHT_LED(4), RGB_RIGHT_LED(5)};
static const uint8_t mouse_accel_keys_right[] = {RGB_RIGHT_LED(20), RGB_RIGHT_LED(21), RGB_RIGHT_LED(22)};

static const uint8_t alpha_keys_both[] = {
    13, 14, 15, 16, 17, 19, 20, 21, 22, 23, 25, 26, 27, 28, 29, RGB_RIGHT_LED(13), RGB_RIGHT_LED(14), RGB_RIGHT_LED(15), RGB_RIGHT_LED(16), RGB_RIGHT_LED(17), RGB_RIGHT_LED(19), RGB_RIGHT_LED(20), RGB_RIGHT_LED(21), RGB_RIGHT_LED(22), RGB_RIGHT_LED(23), RGB_RIGHT_LED(25), RGB_RIGHT_LED(26), RGB_RIGHT_LED(27), RGB_RIGHT_LED(28), RGB_RIGHT_LED(29),
};
static const uint8_t base_layer_keys_left[] = {15, 21, 27};

static const uint8_t oneshot_shift_underglow_both[] = {LED_ONESHOT_SHIFT_UNDERGLOW, RGB_RIGHT_LED(LED_ONESHOT_SHIFT_UNDERGLOW)};
static const uint8_t oneshot_shift_thumb_both[]     = {11, RGB_RIGHT_LED(11)};

// Layer Configurations
const layer_rgb_config_t num_layer_config   = {(const led_rgb_config_t[]){RGB_CONFIG_ARRAY(85, 255, 100, num_pad_right, ARRAY_SIZE(num_pad_right)), // Green numpad keys
                                                                          RGB_CONFIG_SINGLE(85, 255, 100, 7)},
                                               2};
const layer_rgb_config_t mouse_layer_config = {(const led_rgb_config_t[]){
                                                   RGB_CONFIG_ARRAY(43, 255, 175, arrow_keys_left, ARRAY_SIZE(arrow_keys_left)),               // Yellow arrow keys
                                                   RGB_CONFIG_ARRAY(85, 255, 175, mouse_accel_keys_right, ARRAY_SIZE(mouse_accel_keys_right)), // Green acceleration keys
                                                   RGB_CONFIG_SINGLE(43, 255, 175, RGB_RIGHT_LED(LED_THUMB_KEY_3)),                            // Yellow layer indicator
                                                   RGB_CONFIG_ARRAY(43, 255, 100, backplate_both, ARRAY_SIZE(backplate_both)),                 // Yellow backplate
                                               },
                                               4};

const layer_rgb_config_t nav_layer_config = {(const led_rgb_config_t[]){
                                                 RGB_CONFIG_ARRAY(15, 255, 150, arrow_keys_left, ARRAY_SIZE(arrow_keys_left)), // Orange arrow keys
                                                 RGB_CONFIG_SINGLE(15, 255, 150, RGB_RIGHT_LED(LED_LAYER_NAV)),                // Orange layer key (single LED)
                                                 RGB_CONFIG_ARRAY(15, 255, 100, backplate_both, ARRAY_SIZE(backplate_both)),   // Orange backplate
                                             },
                                             3};

static const uint8_t sym_key_right = RGB_RIGHT_LED(7);

static const led_rgb_config_t symbol_layer_configs[] = {
    RGB_CONFIG_ARRAY(200, 255, 150, alpha_keys_both, ARRAY_SIZE(alpha_keys_both)), // Purple symbol keys
    RGB_CONFIG_SINGLE(200, 255, 150, sym_key_right),                               // Purple symbol key
    RGB_CONFIG_ARRAY(200, 255, 100, backplate_both, ARRAY_SIZE(backplate_both))    // Purple backplate
};

const layer_rgb_config_t symbol_layer_config = {symbol_layer_configs, 3};

// Adjust layer configuration
const layer_rgb_config_t adjust_layer_config = {(const led_rgb_config_t[]){
                                                    RGB_CONFIG_SINGLE(0, 255, 100, LED_ADJUST),                                            // Red adjust key
                                                    RGB_CONFIG_ARRAY(85, 255, 175, base_layer_keys_left, ARRAY_SIZE(base_layer_keys_left)) // Green base layers
                                                },
                                                2};

const layer_rgb_config_t caps_word_layer_config = {(const led_rgb_config_t[]){
                                                       RGB_CONFIG_SINGLE(20, 255, 100, LED_CAPS_WORD),   // Red caps word key
                                                       RGB_CONFIG_SINGLE(20, 255, 100, LED_CW_UNDERGLOW) // Red underglow LED
                                                   },
                                                   2};

const layer_rgb_config_t oneshot_shift_layer_config = {(const led_rgb_config_t[]){RGB_CONFIG_ARRAY(20, 255, 100, oneshot_shift_underglow_both, ARRAY_SIZE(oneshot_shift_underglow_both)), // Red underglow for oneshot shift
                                                                                  RGB_CONFIG_ARRAY(20, 255, 50, oneshot_shift_thumb_both, ARRAY_SIZE(oneshot_shift_thumb_both))},
                                                       2};

const layer_rgb_config_t leader_layer_config = {(const led_rgb_config_t[]){RGB_CONFIG_SINGLE(20, 255, 100, RGB_RIGHT_LED(LED_LEADER)), RGB_CONFIG_SINGLE(20, 255, 50, RGB_RIGHT_LED(LED_LEADER_UNDERGLOW))}, 2};

const layer_rgb_config_t sentence_case_layer_config = {(const led_rgb_config_t[]){RGB_CONFIG_SINGLE(20, 255, 100, RGB_RIGHT_LED(LED_ADJUST)), RGB_CONFIG_SINGLE(20, 255, 50, RGB_RIGHT_LED(LED_CW_UNDERGLOW))}, 2};

const layer_rgb_config_t sentence_case_primed_layer_config = {(const led_rgb_config_t[]){RGB_CONFIG_SINGLE(20, 255, 150, RGB_RIGHT_LED(LED_ADJUST)), RGB_CONFIG_SINGLE(20, 255, 100, RGB_RIGHT_LED(LED_CW_UNDERGLOW))}, 2};

// Single-config layers for special scenarios
const layer_rgb_config_t base_layer_config = {(const led_rgb_config_t[]){
                                                  RGB_CONFIG_ALL(0, 0, 100) // Low white for base layer
                                              },
                                              1};

const layer_rgb_config_t bootloader_config = {(const led_rgb_config_t[]){RGB_CONFIG_ALL(0, 255, 100)}, 1};

// RGB utility functions
static void set_rgb_by_key_indices(const uint8_t *indices, uint8_t array_size, rgb_t color) {
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

        hsv_t hsv   = {current_config->hue, current_config->saturation, current_config->value};
        rgb_t color = hsv_to_rgb(hsv);

        switch (current_config->type) {
            case RGB_TYPE_SINGLE:
                // Handle single LED
                rgb_matrix_set_color(current_config->data.single.led_index, color.r, color.g, color.b);
                break;
            case RGB_TYPE_ARRAY:
                // Handle array of LEDs
                set_rgb_by_key_indices(current_config->data.array.led_indices, current_config->data.array.led_count, color);
                break;
            case RGB_TYPE_ALL:
                // Handle all LEDs
                rgb_matrix_set_color_all(color.r, color.g, color.b);
                break;
        }
    }
}
