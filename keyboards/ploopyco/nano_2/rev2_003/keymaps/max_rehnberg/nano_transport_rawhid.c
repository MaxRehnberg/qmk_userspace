#include "nano_core.h"

#if defined(RAW_ENABLE)
#    include "raw_hid.h"

static uint8_t nano_raw_event_sequence = 0;

void nano_transport_raw_send_event(uint8_t event_id, uint16_t arg) {
    uint8_t packet[NANO_RAW_PACKET_SIZE] = {0};

    packet[0] = NANO_RAW_MAGIC_0;
    packet[1] = NANO_RAW_MAGIC_1;
    packet[2] = NANO_RAW_VERSION;
    packet[3] = NANO_RAW_TYPE_EVENT;
    packet[4] = event_id;
    packet[5] = (uint8_t)(arg & 0xFFu);
    packet[6] = (uint8_t)((arg >> 8u) & 0xFFu);
    packet[7] = ++nano_raw_event_sequence;

    raw_hid_send(packet, NANO_RAW_PACKET_SIZE);
}

void nano_transport_raw_receive(uint8_t *data, uint8_t length) {
    uint16_t arg;

    if (!nano_accept_raw_commands()) {
        return;
    }

    if (length != NANO_RAW_PACKET_SIZE) {
        return;
    }

    if (data[0] != NANO_RAW_MAGIC_0 || data[1] != NANO_RAW_MAGIC_1) {
        return;
    }

    if (data[2] != NANO_RAW_VERSION || data[3] != NANO_RAW_TYPE_COMMAND) {
        return;
    }

    arg = (uint16_t)data[5] | ((uint16_t)data[6] << 8u);
    nano_handle_command(data[4], arg);
}

#endif
