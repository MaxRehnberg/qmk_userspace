#ifndef ARCANE_H
#define ARCANE_H

#include <stdint.h>
#include "action.h"

// Function declarations for arcane key processing
void process_left_arcane(uint16_t keycode, uint8_t mods);
void process_right_arcane(uint16_t keycode, uint8_t mods);

#endif // ARCANE_H
