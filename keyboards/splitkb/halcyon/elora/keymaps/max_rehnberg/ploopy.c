#include "ploopy.h"

#if defined(LAYER_LOCK_ENABLE)
#    include "layer_lock.h"
#endif

#ifndef PLOOPY_ACTIVITY_TIMEOUT_MS
#    define PLOOPY_ACTIVITY_TIMEOUT_MS 500u
#endif

#ifndef PLOOPY_LED_BEACON_WINDOW_MS
#    define PLOOPY_LED_BEACON_WINDOW_MS 120u
#endif

#define PLOOPY_RAW_MAGIC_0 'P'
#define PLOOPY_RAW_MAGIC_1 'N'
#define PLOOPY_RAW_VERSION 1u
#define PLOOPY_RAW_TYPE_EVENT 0x02u
#define PLOOPY_RAW_PACKET_SIZE 32u

#define PLOOPY_EVENT_MOVE_START 0x81u

static uint8_t  ploopy_mouse_layer          = 0;
static bool     ploopy_auto_mouse_active    = false;
static uint32_t ploopy_last_activity_timer  = 0;
static bool     ploopy_led_scroll_known     = false;
static bool     ploopy_led_last_scroll_lock = false;
static bool     ploopy_led_window_active    = false;
static uint32_t ploopy_led_window_timer     = 0;
static uint8_t  ploopy_led_transition_count = 0;

static bool ploopy_layer_is_locked(void) {
#if defined(LAYER_LOCK_ENABLE)
    return is_layer_locked(ploopy_mouse_layer);
#else
    return false;
#endif
}

static void ploopy_signal_activity(void) {
    if (!is_keyboard_master()) {
        return;
    }

    ploopy_last_activity_timer = timer_read32();
    ploopy_auto_mouse_active   = true;

    if (!layer_state_is(ploopy_mouse_layer)) {
        layer_on(ploopy_mouse_layer);
    }
}

static void ploopy_finalize_led_window(void) {
    if (!ploopy_led_window_active) {
        return;
    }

    if (timer_elapsed32(ploopy_led_window_timer) < PLOOPY_LED_BEACON_WINDOW_MS) {
        return;
    }

    if (ploopy_led_transition_count >= 2u) {
        ploopy_signal_activity();
    }

    ploopy_led_window_active    = false;
    ploopy_led_transition_count = 0;
}

void ploopy_init(uint8_t mouse_layer) {
    ploopy_mouse_layer          = mouse_layer;
    ploopy_auto_mouse_active    = false;
    ploopy_last_activity_timer  = timer_read32();
    ploopy_led_scroll_known     = false;
    ploopy_led_last_scroll_lock = false;
    ploopy_led_window_active    = false;
    ploopy_led_window_timer     = 0;
    ploopy_led_transition_count = 0;
}

void ploopy_task(void) {
    if (!is_keyboard_master()) {
        return;
    }

    ploopy_finalize_led_window();

    if (!ploopy_auto_mouse_active) {
        return;
    }

    if (timer_elapsed32(ploopy_last_activity_timer) < PLOOPY_ACTIVITY_TIMEOUT_MS) {
        return;
    }

    ploopy_auto_mouse_active = false;

    if (ploopy_layer_is_locked()) {
        return;
    }

    layer_off(ploopy_mouse_layer);
}

bool ploopy_led_update(led_t led_state) {
    const bool scroll_lock = led_state.scroll_lock;

    if (!is_keyboard_master()) {
        return true;
    }

    if (!ploopy_led_scroll_known) {
        ploopy_led_scroll_known     = true;
        ploopy_led_last_scroll_lock = scroll_lock;
        return true;
    }

    if (scroll_lock == ploopy_led_last_scroll_lock) {
        return true;
    }

    ploopy_led_last_scroll_lock = scroll_lock;
    ploopy_finalize_led_window();

    if (!ploopy_led_window_active) {
        ploopy_led_window_active = true;
        ploopy_led_window_timer  = timer_read32();
    }

    ploopy_led_transition_count++;

    if (ploopy_led_transition_count >= 2u) {
        ploopy_signal_activity();
        ploopy_led_window_active    = false;
        ploopy_led_transition_count = 0;
    }

    return true;
}

#if defined(RAW_ENABLE)
void ploopy_raw_hid_receive(uint8_t *data, uint8_t length) {
    if (!is_keyboard_master()) {
        return;
    }

    if (length != PLOOPY_RAW_PACKET_SIZE) {
        return;
    }

    if (data[0] != PLOOPY_RAW_MAGIC_0 || data[1] != PLOOPY_RAW_MAGIC_1) {
        return;
    }

    if (data[2] != PLOOPY_RAW_VERSION || data[3] != PLOOPY_RAW_TYPE_EVENT) {
        return;
    }

    if (data[4] != PLOOPY_EVENT_MOVE_START) {
        return;
    }

    ploopy_signal_activity();
}
#endif
