// App switcher functionality for macOS
#include QMK_KEYBOARD_H
#include "timer.h"
#include "app_switcher.h"

bool            app_switcher_active              = false;
static bool     app_switcher_alt_release_pending = false;
static uint16_t app_switcher_timer               = 0;
static uint16_t app_switcher_activation_timer    = 0;
static uint16_t app_switcher_timeout             = APP_SWITCHER_INITIAL_TIMEOUT;
static uint16_t app_switcher_alt_timer           = 0;
// Function to start the app switcher
void start_app_switcher(void) {
    if (app_switcher_alt_release_pending) {
        unregister_code(KC_LALT);
        app_switcher_alt_release_pending = false;
    }

    uint16_t now = timer_read();

    register_code(KC_LGUI);
    tap_code(KC_TAB);
    app_switcher_active           = true;
    app_switcher_timer            = now;
    app_switcher_activation_timer = now;
    app_switcher_timeout          = APP_SWITCHER_INITIAL_TIMEOUT;
}

void end_app_switcher_with_selection(void) {
    if (!app_switcher_active) {
        return;
    }
    register_code(KC_LALT);
    unregister_code(KC_LGUI);
    app_switcher_active              = false;
    app_switcher_alt_release_pending = true;
    app_switcher_alt_timer           = timer_read();
}

void app_switcher_task(void) {
    if (app_switcher_alt_release_pending && timer_elapsed(app_switcher_alt_timer) >= APP_SWITCHER_SELECTION_HOLD) {
        unregister_code(KC_LALT);
        app_switcher_alt_release_pending = false;
    }
}

// Function to navigate within app switcher and reset timer
void navigate_app_switcher(uint16_t direction_key) {
    tap_code(direction_key);
    app_switcher_timer = timer_read(); // Reset timer
    if (timer_elapsed(app_switcher_activation_timer) > APP_SWITCHER_ACTIVATION_GRACE_PERIOD) {
        app_switcher_timeout = APP_SWITCHER_NAVIGATION_TIMEOUT;
    }
}

bool app_switcher_timed_out(void) {
    return app_switcher_active && timer_elapsed(app_switcher_timer) > app_switcher_timeout;
}

void app_switch(uint16_t direction_key) {
    if (!app_switcher_active) {
        start_app_switcher();
    } else {
        navigate_app_switcher(direction_key);
    }
}
