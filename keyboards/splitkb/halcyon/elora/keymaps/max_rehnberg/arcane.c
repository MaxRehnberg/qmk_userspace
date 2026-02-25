// Arcane key functionality
#include QMK_KEYBOARD_H
#include "arcane.h"
#include "gallium_defs.h"
#include "keymap_swedish.h"
#include "send_string_keycodes.h"

#
// An enhanced version of SEND_STRING: if Caps Word is active, the Shift key is
// held while sending the string. Additionally, the last key is set such that if
// the Repeat Key is pressed next, it produces `repeat_keycode`.
#define MAGIC_STRING(str, repeat_keycode) magic_send_string_P(PSTR(str), (repeat_keycode))

static void magic_send_string_P(const char *str, uint16_t repeat_keycode) {
    uint8_t saved_mods = 0;

    if (is_caps_word_on()) { // If Caps Word is on, save the mods and hold Shift.
        saved_mods = get_mods();
        register_mods(MOD_BIT(KC_LSFT));
    }

    send_string_with_delay_P(str, TAP_CODE_DELAY); // Send the string.
    set_last_keycode(repeat_keycode);

    // If Caps Word is on, restore the mods.
    if (is_caps_word_on()) {
        set_mods(saved_mods);
    }
}

void process_right_arcane(uint16_t keycode, uint8_t mods) { // RARCANE definitions
    switch (keycode) {
        // left side: repeat
        case SE_B: {
            MAGIC_STRING("b", SE_B);
        } break;
        case SE_L: {
            MAGIC_STRING("l", SE_L);
        } break;
        case SE_D: {
            MAGIC_STRING("d", SE_D);
        } break;
        case SE_W: {
            MAGIC_STRING("w", SE_W);
        } break;
        case SE_V: {
            MAGIC_STRING("v", SE_V);
        } break;

        case GH_LHM1: {
            MAGIC_STRING("n", GH_LHM1);
        } break;
        case GH_LHM2: {
            MAGIC_STRING("r", GH_LHM2);
        } break;
        case GH_LHM3: {
            MAGIC_STRING("t", GH_LHM3);
        } break;
        case GH_LHM4: {
            MAGIC_STRING("c", GH_LHM4);
        } break;
        case SE_G: {
            MAGIC_STRING("g", SE_G);
        } break;

        case SE_X: {
            MAGIC_STRING("x", SE_X);
        } break;
        case SE_Q: {
            MAGIC_STRING("q", SE_Q);
        } break;
        case SE_M: {
            MAGIC_STRING("m", SE_M);
        } break;
        case SE_P: {
            MAGIC_STRING("p", SE_P);
        } break;
        case SE_Z: {
            MAGIC_STRING("z", SE_Z);
        } break;

        case SE_S: {
            MAGIC_STRING("s", SE_S);
        } break;

        // right side: magic
        case SE_J: {
            MAGIC_STRING(SS_TAP(X_QUOT), SE_ADIA);
        } break;
        case SE_Y: {
            MAGIC_STRING("y", SE_Y);
        } break;
        case SE_O: {
            MAGIC_STRING("a", GH_RHM3);
        } break;
        case SE_U: {
            MAGIC_STRING("e", GH_RHM2);
        } break;
        case SE_EXLM: {
            MAGIC_STRING("!", SE_EXLM);
        } break;

        case SE_K: {
            MAGIC_STRING("y", SE_Y);
        } break;
        case GH_RHM4: {
            MAGIC_STRING("y", SE_Y);
        } break;
        case GH_RHM3: {
            MAGIC_STRING("o", SE_O);
        } break;
        case GH_RHM2: {
            MAGIC_STRING("u", SE_U);
        } break;
        case GH_RHM1: {
            MAGIC_STRING("i", GH_RHM1);
        } break;

        case SE_F: {
            MAGIC_STRING("y", SE_Y);
        } break;

        // default case, repeat
        default:
            tap_code16(keycode);
            break;
    }
}

void process_left_arcane(uint16_t keycode, uint8_t mods) { // LARCANE definitions
    switch (keycode) {
        // left side: magic
        case SE_B: {
            MAGIC_STRING("j", SE_J);
        } break;
        case SE_L: {
            MAGIC_STRING("r", GH_LHM2);
        } break;
        case SE_D: {
            MAGIC_STRING("m", SE_B);
        } break;
        case SE_W: {
            MAGIC_STRING("q", SE_Q);
        } break;
        case SE_V: {
            MAGIC_STRING("v", SE_V);
        } break;

        case GH_LHM1: {
            MAGIC_STRING("n", GH_LHM1);
        } break;
        case GH_LHM2: {
            MAGIC_STRING("l", SE_L);
        } break;
        case GH_LHM3: {
            MAGIC_STRING("m", SE_M);
        } break;
        case GH_LHM4: {
            MAGIC_STRING("c", GH_LHM4);
        } break;
        case SE_G: {
            MAGIC_STRING("g", SE_G);
        } break;

        case SE_X: {
            MAGIC_STRING("x", SE_X);
        } break;
        case SE_Q: {
            MAGIC_STRING("l", SE_L);
        } break;
        case SE_M: {
            MAGIC_STRING("b", SE_B);
        } break;
        case SE_P: {
            MAGIC_STRING("p", SE_P);
        } break;
        case SE_Z: {
            MAGIC_STRING("z", SE_Z);
        } break;

        case SE_S: {
            MAGIC_STRING("s", SE_S);
        } break;

        // right side: repeat
        case SE_J: {
            MAGIC_STRING("j", SE_J);
        } break;
        case SE_Y: {
            MAGIC_STRING("y", SE_Y);
        } break;
        case SE_O: {
            MAGIC_STRING("o", SE_O);
        } break;
        case SE_U: {
            MAGIC_STRING("u", SE_U);
        } break;
        case SE_EXLM: {
            MAGIC_STRING("!", SE_EXLM);
        } break;

        case SE_K: {
            MAGIC_STRING("k", SE_K);
        } break;
        case GH_RHM4: {
            MAGIC_STRING("h", GH_RHM4);
        } break;
        case GH_RHM3: {
            MAGIC_STRING("a", GH_RHM3);
        } break;
        case GH_RHM2: {
            MAGIC_STRING("e", GH_RHM2);
        } break;
        case GH_RHM1: {
            MAGIC_STRING("i", GH_RHM1);
        } break;

        case SE_F: {
            MAGIC_STRING("f", SE_F);
        } break;
        // default case, repeat
        default:
            tap_code16(keycode);
            break;
    }
}
