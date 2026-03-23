/* Copyright 2021 Colin Lam (Ploopy Corporation)
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2019 Hiroyuki Okada
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "nano_core.h"

enum {
    TD_SCROLL_BOOT,
};

void scroll_boot_finished(tap_dance_state_t *state, void *user_data) {
    (void)user_data;

    if (state->count == 1) {
        nano_handle_command(NANO_CMD_TOGGLE_SCROLL, 0u);
    } else if (state->count >= 3) {
        nano_handle_command(NANO_CMD_BOOTLOADER, 0u);
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_SCROLL_BOOT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, scroll_boot_finished, NULL),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(TD(TD_SCROLL_BOOT)),
};

void keyboard_post_init_user(void) {
    nano_core_post_init();
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    return nano_core_pointing_device_task(mouse_report);
}

void housekeeping_task_user(void) {
    nano_core_housekeeping_task();
}

bool led_update_user(led_t led_state) {
    return nano_transport_led_update(led_state);
}

#if defined(RAW_ENABLE)
void raw_hid_receive(uint8_t *data, uint8_t length) {
    nano_transport_raw_receive(data, length);
}
#endif
