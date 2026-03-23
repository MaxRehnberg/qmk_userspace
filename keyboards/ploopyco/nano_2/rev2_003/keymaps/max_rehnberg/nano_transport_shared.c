#include "nano_core.h"

#if defined(OS_DETECTION_ENABLE)
#    include "os_detection.h"
#endif

enum nano_transport_mode nano_get_transport_mode(void) {
#if defined(OS_DETECTION_ENABLE)
    switch (detected_host_os()) {
        case OS_WINDOWS:
        case OS_LINUX:
            return NANO_TRANSPORT_LED;
        case OS_MACOS:
            return NANO_TRANSPORT_RAW;
        default:
            return NANO_TRANSPORT_FALLBACK;
    }
#else
    return NANO_TRANSPORT_FALLBACK;
#endif
}

bool nano_accept_led_commands(void) {
    enum nano_transport_mode mode = nano_get_transport_mode();
    return mode == NANO_TRANSPORT_LED || mode == NANO_TRANSPORT_FALLBACK;
}

bool nano_accept_raw_commands(void) {
    enum nano_transport_mode mode = nano_get_transport_mode();
    return mode == NANO_TRANSPORT_RAW || mode == NANO_TRANSPORT_FALLBACK;
}

bool nano_emit_led_beacon_enabled(void) {
    return nano_get_transport_mode() == NANO_TRANSPORT_LED;
}
