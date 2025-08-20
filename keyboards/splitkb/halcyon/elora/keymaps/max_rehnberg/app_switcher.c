// App switcher functionality for macOS
#include QMK_KEYBOARD_H
#include "timer.h"
#include "wait.h"
#include "app_switcher.h"

bool     app_switcher_active = false;
uint16_t app_switcher_timer  = 0;
// Function to start the app switcher
void start_app_switcher(void) {
    register_code(KC_LGUI);
    tap_code(KC_TAB);
    app_switcher_active = true;
    app_switcher_timer  = timer_read();
}

void end_app_switcher_with_selection(void) {
    if (!app_switcher_active) {
        return;
    }
    register_code(KC_LALT);
    unregister_code(KC_LGUI);
    wait_ms(50); // Brief hold
    unregister_code(KC_LALT);
    app_switcher_active = false;
}

// Function to navigate within app switcher and reset timer
void navigate_app_switcher(uint16_t direction_key) {
    tap_code(direction_key);
    app_switcher_timer = timer_read(); // Reset timer
}

void app_switch(uint16_t direction_key) {
    if (!app_switcher_active) {
        start_app_switcher();
    } else {
        navigate_app_switcher(direction_key);
    }
}