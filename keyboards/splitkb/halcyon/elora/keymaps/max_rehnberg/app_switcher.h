#ifndef APP_SWITCHER_H
#define APP_SWITCHER_H

#include <stdint.h>
#include <stdbool.h>

extern bool app_switcher_active;

// Function declarations
void start_app_switcher(void);
void navigate_app_switcher(uint16_t direction_key);
void app_switch(uint16_t direction_key);
void end_app_switcher_with_selection(void);
bool app_switcher_timed_out(void);

#endif // APP_SWITCHER_H
