// Arcane key functionality
#include QMK_KEYBOARD_H
#include "arcane.h"
#include "keymap_swedish.h"
#include "select_word.h"

// An enhanced version of SEND_STRING: if Caps Word is active, the Shift key is
// held while sending the string. Additionally, the last key is set such that if
// the Repeat Key is pressed next, it produces `repeat_keycode`.
#define MAGIC_STRING(str, repeat_keycode) \
        magic_send_string_P(PSTR(str), (repeat_keycode))
 
static void magic_send_string_P(const char* str, uint16_t repeat_keycode) {
    uint8_t saved_mods = 0;
 
    if (is_caps_word_on()) { // If Caps Word is on, save the mods and hold Shift.
        saved_mods = get_mods();
        register_mods(MOD_BIT(KC_LSFT));
    }
 
    send_string_with_delay_P(str, TAP_CODE_DELAY);  // Send the string.
    set_last_keycode(repeat_keycode);
 
    // If Caps Word is on, restore the mods.
    if (is_caps_word_on()) {
        set_mods(saved_mods);
    }
}

// Home row mod definitions from keymap
#define GH_LCTL MT(MOD_LCTL, SE_N)
#define GH_LALT MT(MOD_LALT, SE_R)
#define GH_LSFT MT(MOD_LSFT, SE_T)
#define GH_LGUI MT(MOD_LGUI, SE_C)
#define GH_RGUI MT(MOD_RGUI, SE_H)
#define GH_RSFT MT(MOD_RSFT, SE_A)
#define GH_RALT MT(MOD_RALT, SE_E)
#define GH_RCTL MT(MOD_RCTL, SE_I)
#define MEH_S MT(MOD_LCTL | MOD_LALT | MOD_LSFT, SE_S)

void process_right_arcane(uint16_t keycode, uint8_t mods) { // RARCANE definitions
    switch (keycode) {
        // left side: repeat
        case SE_B: { MAGIC_STRING("b",       SE_B); } break;
        case SE_L: { MAGIC_STRING("l",       SE_L); } break;
        case SE_D: { MAGIC_STRING("d",       SE_D); } break;
        case SE_W: { MAGIC_STRING("w",       SE_W); } break;
        case SE_V: { MAGIC_STRING("v",       SE_V); } break;

        case GH_LCTL: { MAGIC_STRING("n",    GH_LCTL); } break;
        case GH_LALT: { MAGIC_STRING("r",    GH_LALT); } break;
        case GH_LSFT: { MAGIC_STRING("t",    GH_LSFT); } break;
        case GH_LGUI: { MAGIC_STRING("c",    GH_LGUI); } break;
        case SE_G: { MAGIC_STRING("g",       SE_G); } break;

        case SE_X: { MAGIC_STRING("x",       SE_X); } break;
        case SE_Q: { MAGIC_STRING("q",       SE_Q); } break;
        case SE_M: { MAGIC_STRING("m",       SE_M); } break;
        case SE_P: { MAGIC_STRING("p",       SE_P); } break;
        case SE_Z: { MAGIC_STRING("z",       SE_Z); } break;
        
        case MEH_S: { MAGIC_STRING("s",       MEH_S); } break;
        
        // right side: magic
        case SE_J: { MAGIC_STRING("j",       SE_ADIA); } break;
        case SE_Y: { MAGIC_STRING("y",       SE_Y); } break;
        case SE_O: { MAGIC_STRING("f",       SE_F); } break;
        case SE_U: { MAGIC_STRING("e",       SE_E); } break;
        case SE_EXLM: { MAGIC_STRING("!",    SE_EXLM); } break;
        
        case SE_K: { MAGIC_STRING("y",       SE_Y); } break;
        case GH_RGUI: { MAGIC_STRING("y",    GH_RGUI); } break;
        case GH_RSFT: { MAGIC_STRING("a",    GH_RSFT); } break;
        case GH_RALT: { MAGIC_STRING("u",    GH_RALT); } break;
        case GH_RCTL: { MAGIC_STRING("i",    GH_RCTL); } break;

        case SE_F: { MAGIC_STRING("o",       SE_O); } break;

        // Select word keycodes
        case SELWORD:
            select_word_tap('W');
            set_last_keycode(SELWORD);
            break;
        case SELWBAK:
            select_word_tap('B');
            set_last_keycode(SELWBAK);
            break;
        case SELLINE:
            select_word_tap('L');
            set_last_keycode(SELLINE);
            break;

        // default case, repeat
        default:
            tap_code16(keycode);
            break;
    }
}

void process_left_arcane(uint16_t keycode, uint8_t mods) { // LARCANE definitions
    switch (keycode) {
        // left side: magic
        case SE_B: { MAGIC_STRING("j",       SE_J); } break;
        case SE_L: { MAGIC_STRING("r",       GH_LALT); } break;
        case SE_D: { MAGIC_STRING("m",       SE_B); } break;
        case SE_W: { MAGIC_STRING("w",       SE_W); } break;
        case SE_V: { MAGIC_STRING("v",       SE_V); } break;

        case GH_LCTL: { MAGIC_STRING("n",    GH_LCTL); } break;
        case GH_LALT: { MAGIC_STRING("l",    SE_L); } break;
        case GH_LSFT: { MAGIC_STRING("m",    SE_M); } break;
        case GH_LGUI: { MAGIC_STRING("c",    GH_LGUI); } break;
        case SE_G: { MAGIC_STRING("n",       SE_N); } break;

        case SE_X: { MAGIC_STRING("x",       SE_X); } break;
        case SE_Q: { MAGIC_STRING("q",       SE_Q); } break;
        case SE_M: { MAGIC_STRING("b",       SE_B); } break;
        case SE_P: { MAGIC_STRING("p",       SE_P); } break;
        case SE_Z: { MAGIC_STRING("z",       SE_Z); } break;
        
        case MEH_S: { MAGIC_STRING("s",       MEH_S); } break;
        
        // right side: repeat
        case SE_J: { MAGIC_STRING("j",       SE_J); } break;
        case SE_Y: { MAGIC_STRING("y",       SE_Y); } break;
        case SE_O: { MAGIC_STRING("o",       SE_O); } break;
        case SE_U: { MAGIC_STRING("u",       SE_U); } break;
        case SE_EXLM: { MAGIC_STRING("!",    SE_EXLM); } break;
        
        case SE_K: { MAGIC_STRING("k",       SE_K); } break;
        case GH_RGUI: { MAGIC_STRING("h",    GH_RGUI); } break;
        case GH_RSFT: { MAGIC_STRING("a",    GH_RSFT); } break;
        case GH_RALT: { MAGIC_STRING("e",    GH_RALT); } break;
        case GH_RCTL: { MAGIC_STRING("i",    GH_RCTL); } break;
        
        case SE_F: { MAGIC_STRING("f",       SE_F); } break;

        // Select word keycodes
        case SELWORD:
            select_word_tap('W');
            set_last_keycode(SELWORD);
            break;
        case SELWBAK:
            select_word_tap('B');
            set_last_keycode(SELWBAK);
            break;
        case SELLINE:
            select_word_tap('L');
            set_last_keycode(SELLINE);
            break;

        // default case, repeat
        default:
            tap_code16(keycode);
            break;
    }
}
