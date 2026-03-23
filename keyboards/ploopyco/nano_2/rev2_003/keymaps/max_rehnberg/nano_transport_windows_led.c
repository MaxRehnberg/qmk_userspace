#include "nano_core.h"

#define NANO_LED_COMMAND_WINDOW_MS 120u
#define NANO_LED_SELF_GUARD_MS 250u
#define NANO_LED_BEACON_COOLDOWN_MS 300u

static uint32_t nano_last_beacon_timer     = 0;
static bool     nano_led_scroll_lock_known = false;
static bool     nano_led_last_scroll_lock  = false;
static bool     nano_led_window_active     = false;
static uint32_t nano_led_window_timer      = 0;
static uint8_t  nano_led_transition_count  = 0;
static bool     nano_led_self_guard_on     = false;
static uint32_t nano_led_self_guard_timer  = 0;

static bool nano_led_in_self_guard(void) {
    if (!nano_led_self_guard_on) {
        return false;
    }

    if (timer_elapsed32(nano_led_self_guard_timer) >= NANO_LED_SELF_GUARD_MS) {
        nano_led_self_guard_on = false;
        return false;
    }

    return true;
}

static void nano_run_led_command(uint8_t transitions) {
    if ((transitions % 2u) != 0u) {
        return;
    }

    switch (transitions) {
        case 2:
            nano_handle_command(NANO_CMD_TOGGLE_SCROLL, 0u);
            break;
        case 4:
            nano_handle_command(NANO_CMD_CYCLE_DPI, 0u);
            break;
        case 6:
            nano_handle_command(NANO_CMD_BOOTLOADER, 0u);
            break;
        default:
            break;
    }
}

static void nano_finalize_led_window(void) {
    uint8_t transitions;

    if (!nano_led_window_active) {
        return;
    }

    if (timer_elapsed32(nano_led_window_timer) < NANO_LED_COMMAND_WINDOW_MS) {
        return;
    }

    transitions = nano_led_transition_count;

    nano_led_window_active    = false;
    nano_led_transition_count = 0;

    if (!nano_accept_led_commands()) {
        return;
    }

    nano_run_led_command(transitions);
}

void nano_transport_emit_led_beacon(void) {
    if (!nano_emit_led_beacon_enabled()) {
        return;
    }

    if (timer_elapsed32(nano_last_beacon_timer) < NANO_LED_BEACON_COOLDOWN_MS) {
        return;
    }

    nano_last_beacon_timer    = timer_read32();
    nano_led_self_guard_on    = true;
    nano_led_self_guard_timer = nano_last_beacon_timer;

    tap_code16(KC_SCRL);
    wait_ms(8);
    tap_code16(KC_SCRL);
}

bool nano_transport_led_update(led_t led_state) {
    bool scroll_lock = led_state.scroll_lock;

    if (!nano_led_scroll_lock_known) {
        nano_led_scroll_lock_known = true;
        nano_led_last_scroll_lock  = scroll_lock;
        return true;
    }

    if (scroll_lock == nano_led_last_scroll_lock) {
        return true;
    }

    nano_led_last_scroll_lock = scroll_lock;

    if (nano_led_in_self_guard()) {
        return true;
    }

    if (!nano_accept_led_commands()) {
        return true;
    }

    if (nano_led_window_active && timer_elapsed32(nano_led_window_timer) >= NANO_LED_COMMAND_WINDOW_MS) {
        nano_run_led_command(nano_led_transition_count);
        nano_led_transition_count = 0;
        nano_led_window_timer     = timer_read32();
    } else if (!nano_led_window_active) {
        nano_led_window_active = true;
        nano_led_window_timer  = timer_read32();
    }

    nano_led_transition_count++;

    return true;
}

void nano_transport_led_housekeeping(void) {
    nano_finalize_led_window();
}
