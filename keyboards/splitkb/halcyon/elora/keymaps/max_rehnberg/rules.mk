ENCODER_MAP_ENABLE = yes
REPEAT_KEY_ENABLE = yes
CAPS_WORD_ENABLE = yes
COMBO_ENABLE = yes
LAYER_LOCK_ENABLE = yes
MOUSE_KEY_ENABLE = yes
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = ws2812
SPLIT_KEYBOARD = yes


# Add custom RGB configuration
SRC += rgb_config.c

# Add app switcher functionality
SRC += app_switcher.c

# This adds module functionality to your keyboard (files found in users/halcyon_modules)
USER_NAME := halcyon_modules
