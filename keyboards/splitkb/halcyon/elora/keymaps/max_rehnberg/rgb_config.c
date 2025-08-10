// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "rgb_config.h"
#define RGB_MATRIX_SPLIT { 37, 37 }
#define SPLIT_LAYER_STATE_ENABLE

// Shared LED Groups

static const uint8_t arrow_keys[] = {LED_ARROW_UP, LED_ARROW_DOWN, LED_ARROW_LEFT, LED_ARROW_RIGHT};

// Common key groups that could be reused across layers
//static const uint8_t number_row[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // 1-0 and Esc keys (if needed)


// Layer Configurations
static const uint8_t mouse_accel_keys[] = {24, 29, 30};
const layer_rgb_config_t mouse_layer_config = {
    (const led_rgb_config_t[]){
        RGB_CONFIG_ARRAY(43, 255, 175, arrow_keys, 4),      // Yellow arrow keys
        RGB_CONFIG_ARRAY(85, 255, 175, mouse_accel_keys, 3), // Green acceleration keys
        RGB_CONFIG_SINGLE(43, 255, 175, LED_THUMB_KEY_1) // Yellow layer indicator
    },
    2 // number of configs
};

const layer_rgb_config_t nav_layer_config = {
    (const led_rgb_config_t[]){
        RGB_CONFIG_ARRAY(4, 255, 175, arrow_keys, 4),       // Orange arrow keys
        RGB_CONFIG_SINGLE(4, 255, 175, LED_LAYER_NAV)    // Orange layer key (single LED)
    },
    2
};

const layer_rgb_config_t symbol_layer_config = {
    (const led_rgb_config_t[]){
        RGB_CONFIG_ALL(200, 255, 150),                        // Dim purple base for all keys
    },
    1
};

// Adjust layer configuration
static const uint8_t base_layer_keys[] = {15 , 21, 27};
const layer_rgb_config_t adjust_layer_config = {
    (const led_rgb_config_t[]){
        RGB_CONFIG_SINGLE(0, 255, 100, LED_ADJUST),       // Red adjust key
        RGB_CONFIG_ARRAY(85, 255, 175, base_layer_keys, 3)    // Green base layers
    },
    2
};

const layer_rgb_config_t caps_word_layer_config = {
    (const led_rgb_config_t[]){
        RGB_CONFIG_SINGLE(0, 255, 100, LED_CAPS_WORD),   // Red caps word key
        RGB_CONFIG_SINGLE(0, 255, 100, LED_CW_UNDERGLOW) // Red underglow LED
    },
    2
};

// Single-config layers for special scenarios
const layer_rgb_config_t base_layer_config = {
    (const led_rgb_config_t[]){
        RGB_CONFIG_ALL(0, 0, 100)        // Low white for base layer
    },
    1
};

const layer_rgb_config_t bootloader_config = {
    (const led_rgb_config_t[]){
        RGB_CONFIG_ALL(0, 255, 100)
    },
    1
};

// RGB utility functions
void set_hsv_by_key_indices(const uint8_t *indices, uint8_t array_size, hsv_t hsv) {
    rgb_t color = hsv_to_rgb(hsv);
    for (uint8_t i = 0; i < array_size; i++) {
        rgb_matrix_set_color(indices[i], color.r, color.g, color.b);
    }
}

// Generic function to apply multiple RGB configs to a layer
void set_layer_rgb_by_configs(const layer_rgb_config_t* config) {
    for (uint8_t i = 0; i < config->config_count; i++) {
        const led_rgb_config_t* current_config = &config->configs[i];
        hsv_t hsv = {current_config->hue, current_config->saturation, current_config->value};
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

void set_caps_word_rgb(hsv_t hsv, uint8_t led_min, uint8_t led_max) {
    hsv.h = 0; // red
    hsv.s = 255;
    rgb_t caps_word_rgb = hsv_to_rgb(hsv);
    RGB_MATRIX_INDICATOR_SET_COLOR(LED_CW_UNDERGLOW, caps_word_rgb.r, caps_word_rgb.g, caps_word_rgb.b); // Underglow LED
    RGB_MATRIX_INDICATOR_SET_COLOR(LED_CAPS_WORD, caps_word_rgb.r, caps_word_rgb.g, caps_word_rgb.b); // Key
}
