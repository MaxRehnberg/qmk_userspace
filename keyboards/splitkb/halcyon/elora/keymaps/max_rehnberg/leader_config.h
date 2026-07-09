#pragma once

#include <stdbool.h>
#include <stdint.h>

bool keymap_leader_is_active(void);
bool keymap_leader_is_active_synced(void);
void keymap_leader_sync_handler(uint8_t in_size, const void *in_data, uint8_t out_size, void *out_data);
