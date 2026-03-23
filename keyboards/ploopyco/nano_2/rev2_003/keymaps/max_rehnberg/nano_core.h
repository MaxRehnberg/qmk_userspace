#pragma once

#include QMK_KEYBOARD_H

enum nano_command_id {
    NANO_CMD_TOGGLE_SCROLL = 0x01,
    NANO_CMD_BOOTLOADER    = 0x02,
    NANO_CMD_CYCLE_DPI     = 0x03,
};

enum nano_event_id {
    NANO_EVENT_MOVE_START   = 0x81,
    NANO_EVENT_MOVE_STOP    = 0x82,
    NANO_EVENT_SCROLL_STATE = 0x83,
};

enum nano_transport_mode {
    NANO_TRANSPORT_FALLBACK = 0,
    NANO_TRANSPORT_LED,
    NANO_TRANSPORT_RAW,
};

#define NANO_RAW_MAGIC_0 'P'
#define NANO_RAW_MAGIC_1 'N'
#define NANO_RAW_VERSION 1u
#define NANO_RAW_TYPE_COMMAND 0x01
#define NANO_RAW_TYPE_EVENT 0x02
#define NANO_RAW_PACKET_SIZE 32u

enum nano_transport_mode nano_get_transport_mode(void);
bool                     nano_accept_led_commands(void);
bool                     nano_accept_raw_commands(void);
bool                     nano_emit_led_beacon_enabled(void);

void           nano_core_post_init(void);
report_mouse_t nano_core_pointing_device_task(report_mouse_t mouse_report);
void           nano_core_housekeeping_task(void);

void nano_handle_command(uint8_t command_id, uint16_t arg);
void nano_emit_event(uint8_t event_id, uint16_t arg);

bool nano_transport_led_update(led_t led_state);
void nano_transport_led_housekeeping(void);
void nano_transport_emit_led_beacon(void);

#if defined(RAW_ENABLE)
void nano_transport_raw_receive(uint8_t *data, uint8_t length);
void nano_transport_raw_send_event(uint8_t event_id, uint16_t arg);
#endif
