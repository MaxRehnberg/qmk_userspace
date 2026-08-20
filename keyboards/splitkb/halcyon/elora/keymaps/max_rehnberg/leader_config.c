#include QMK_KEYBOARD_H

#include "leader_config.h"

#include "keymap_swedish.h"
#include "leader.h"
#include "sendstring_swedish.h"
#include "xcase.h"

#include <string.h>

static bool    is_leader_active     = false;
static uint8_t leader_active_synced = 0;

static void toggle_xcase(uint16_t keycode) {
    if (is_xcase_active()) {
        disable_xcase();
    } else {
        enable_xcase_with(keycode);
    }
}

static void double_tap(uint16_t keycode) {
    tap_code16(keycode);
    wait_ms(50);
    tap_code16(keycode);
}

static void send_parent_path(uint8_t levels) {
    for (uint8_t i = 0; i < levels; i++) {
        SEND_STRING("../");
    }
}

bool keymap_leader_is_active(void) {
    return is_leader_active;
}

bool keymap_leader_is_active_synced(void) {
    return leader_active_synced != 0;
}

void keymap_leader_sync_handler(uint8_t in_size, const void *in_data, uint8_t out_size, void *out_data) {
    if (in_size == sizeof(leader_active_synced)) {
        memcpy(&leader_active_synced, in_data, sizeof(leader_active_synced));
    }
}

void leader_start_user(void) {
    is_leader_active     = true;
    leader_active_synced = 1;
}

void leader_end_user(void) {
    if (leader_sequence_one_key(SE_AT) || leader_sequence_two_keys(KC_A, KC_T)) {
        SEND_STRING("max.rehnberg@odevo.com");
    } else if (leader_sequence_one_key(KC_E)) {
        // é
        tap_code16(SE_ACUT);
        tap_code16(KC_E);
    } else if (leader_sequence_two_keys(KC_T, KC_E)) {
        // É
        tap_code16(SE_ACUT);
        tap_code16(S(KC_E));
    } else if (leader_sequence_one_key(KC_F)) {
        // send f"" for Python f-strings and place cursor in between
        tap_code(KC_F);
        double_tap(SE_DQUO);
        tap_code(KC_LEFT);
    } else if (leader_sequence_two_keys(KC_E, KC_Q)) {
        // ==
        double_tap(SE_EQL);
    } else if (leader_sequence_two_keys(KC_N, KC_E)) {
        // !=
        tap_code16(SE_EXLM);
        tap_code16(SE_EQL);
    } else if (leader_sequence_two_keys(KC_G, KC_E)) {
        // >=
        tap_code16(S(KC_GRV));
        tap_code16(SE_EQL);
    } else if (leader_sequence_two_keys(KC_L, KC_E)) {
        // <=
        tap_code16(KC_GRV);
        tap_code16(SE_EQL);
    } else if (leader_sequence_two_keys(KC_A, KC_R)) {
        // -> (arrow)
        tap_code16(SE_MINS);
        tap_code16(S(KC_GRV));
    } else if (leader_sequence_two_keys(SE_E, SE_P)) {
        // exit Python PDB
        SEND_STRING("import os");
        tap_code16(KC_ENT);
        SEND_STRING("os._exit(0)");
        tap_code16(KC_ENT);
    } else if (leader_sequence_two_keys(SE_C, SE_S)) {
        // Snake case
        toggle_xcase(SE_UNDS);
    } else if (leader_sequence_two_keys(SE_C, SE_C)) {
        // Camel case
        toggle_xcase(KC_LSFT);
    } else if (leader_sequence_two_keys(SE_C, SE_SLSH)) {
        // Path case
        toggle_xcase(SE_SLSH);
    } else if (leader_sequence_two_keys(SE_C, SE_K)) {
        // Kebab case
        toggle_xcase(SE_MINS);
    } else if (leader_sequence_two_keys(SE_C, SE_DOT)) {
        // Dot case
        toggle_xcase(SE_DOT);
    } else if (leader_sequence_three_keys(SE_DOT, SE_DOT, SE_DOT)) {
        // ../../../
        send_parent_path(3);
    } else if (leader_sequence_two_keys(SE_DOT, SE_DOT)) {
        // ../../
        send_parent_path(2);
    } else if (leader_sequence_one_key(SE_DOT)) {
        // ../
        send_parent_path(1);
    }

    is_leader_active     = false;
    leader_active_synced = 0;
}
