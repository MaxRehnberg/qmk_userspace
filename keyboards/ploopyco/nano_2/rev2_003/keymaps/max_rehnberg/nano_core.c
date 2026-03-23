#include "nano_core.h"

void toggle_drag_scroll(void);
void cycle_dpi(void);

static bool     nano_scroll_enabled    = false;
static bool     nano_motion_active     = false;
static uint32_t nano_last_motion_timer = 0;
static uint32_t nano_last_event_timer  = 0;

#define NANO_MOTION_IDLE_TIMEOUT_MS 400u
#define NANO_MOVE_EVENT_PERIOD_MS 120u

static void nano_toggle_scroll(void) {
    toggle_drag_scroll();
    nano_scroll_enabled = !nano_scroll_enabled;
    nano_emit_event(NANO_EVENT_SCROLL_STATE, nano_scroll_enabled ? 1u : 0u);
}

static void nano_cycle_dpi(void) {
    cycle_dpi();
}

static void nano_bootloader(void) {
    reset_keyboard();
}

void nano_handle_command(uint8_t command_id, uint16_t arg) {
    (void)arg;

    switch (command_id) {
        case NANO_CMD_TOGGLE_SCROLL:
            nano_toggle_scroll();
            break;
        case NANO_CMD_BOOTLOADER:
            nano_bootloader();
            break;
        case NANO_CMD_CYCLE_DPI:
            nano_cycle_dpi();
            break;
        default:
            break;
    }
}

void nano_emit_event(uint8_t event_id, uint16_t arg) {
#if defined(RAW_ENABLE)
    if (nano_accept_raw_commands()) {
        nano_transport_raw_send_event(event_id, arg);
    }
#else
    (void)event_id;
    (void)arg;
#endif
}

void nano_core_post_init(void) {
    nano_last_motion_timer = timer_read32();
    nano_last_event_timer  = nano_last_motion_timer;
}

report_mouse_t nano_core_pointing_device_task(report_mouse_t mouse_report) {
    uint32_t now   = timer_read32();
    bool     moved = mouse_report.x != 0 || mouse_report.y != 0 || mouse_report.h != 0 || mouse_report.v != 0;

    if (moved) {
        nano_last_motion_timer = now;

        if (!nano_motion_active) {
            nano_motion_active = true;
            nano_emit_event(NANO_EVENT_MOVE_START, 0u);
            nano_last_event_timer = now;
        } else if (timer_elapsed32(nano_last_event_timer) >= NANO_MOVE_EVENT_PERIOD_MS) {
            nano_emit_event(NANO_EVENT_MOVE_START, 0u);
            nano_last_event_timer = now;
        }

        nano_transport_emit_led_beacon();
    } else if (nano_motion_active && timer_elapsed32(nano_last_motion_timer) >= NANO_MOTION_IDLE_TIMEOUT_MS) {
        nano_motion_active = false;
        nano_emit_event(NANO_EVENT_MOVE_STOP, 0u);
    }

    return mouse_report;
}

void nano_core_housekeeping_task(void) {
    nano_transport_led_housekeeping();
}
