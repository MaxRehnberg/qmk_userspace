#pragma once

#include <stdint.h>
#include <stdbool.h>

// External variables that other files need to access
extern bool app_switcher_active;
extern uint16_t app_switcher_timer;

// Function declarations
void start_app_switcher(void);
void navigate_app_switcher(uint16_t direction_key);
void app_switch(uint16_t direction_key);
void end_app_switcher_with_selection(void);
