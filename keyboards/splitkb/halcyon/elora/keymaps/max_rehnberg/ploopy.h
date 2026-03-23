#pragma once

#include QMK_KEYBOARD_H

void ploopy_init(uint8_t mouse_layer);
void ploopy_task(void);
bool ploopy_led_update(led_t led_state);

#if defined(RAW_ENABLE)
void ploopy_raw_hid_receive(uint8_t *data, uint8_t length);
#endif
