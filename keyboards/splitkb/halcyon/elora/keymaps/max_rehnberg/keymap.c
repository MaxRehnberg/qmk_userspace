// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "action_layer.h"
#include "color.h"
#include "info_config.h"
#include "keycodes.h"
#include "keymap_swedish.h"
#include "keymap_us.h"
#include "modifiers.h"
#include "quantum.h"
#include "quantum_keycodes.h"
#include "rgb_matrix.h"
#include QMK_KEYBOARD_H

bool caps_word_enabled = false;

const uint16_t PROGMEM shift_bspc_del[] = {KC_RSFT, KC_BSPC, COMBO_END};
combo_t key_combos[] = {
    COMBO(shift_bspc_del, KC_DEL), // Shift + Backspace produces Delete
};

enum layers {
    L_QWERTY = 0,
    L_GALLIUM,
    L_GAMING,
    L_NAV,
    L_MS,
    L_SYM,
    L_ADJUST,
};

// Aliases for readability
#define QWERTY DF(L_QWERTY)
#define GALLIUM DF(L_GALLIUM)
#define GAMING DF(L_GAMING)

#define SYM MO(L_SYM)
#define SPC_NAV LT(L_NAV, KC_SPC)
#define TAB_SYM LT(L_SYM, KC_TAB)
#define ENT_MS LT(L_MS, KC_ENT)
#define ADJUST MO(L_ADJUST)

#define CTL_ESC MT(MOD_LCTL, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT MT(MOD_LALT, KC_ENT)



// Define lower home row mods for GUI, shft, alt, ctrl

// QWERTY
// Left side
#define QH_LGUI MT(MOD_LGUI, KC_V)
#define QH_LSFT MT(MOD_LSFT, KC_C)
#define QH_LALT MT(MOD_LALT, KC_X)
#define QH_LCTL MT(MOD_LCTL, KC_Z)
// Right side
#define QH_RGUI MT(MOD_RGUI, KC_M)
#define QH_RSFT MT(MOD_RSFT, KC_COMM)
#define QH_RALT MT(MOD_RALT, KC_DOT)
#define QH_RCTL MT(MOD_RCTL, KC_MINS)

// Gallium
// Left side
#define GH_LGUI MT(MOD_LGUI, KC_W)
#define GH_LSFT MT(MOD_LSFT, KC_M)
#define GH_LALT MT(MOD_LALT, KC_Q)
#define GH_LCTL MT(MOD_LCTL, KC_X)
// Right side
#define GH_RGUI MT(MOD_RGUI, KC_F)
#define GH_RSFT MT(MOD_RSFT, KC_COMM)
#define GH_RALT MT(MOD_RALT, KC_DOT)
#define GH_RCTL MT(MOD_RCTL, KC_MINS)

// Nav
// Right side
#define NV_RGUI MT(MOD_RGUI, KC_1)
#define NV_RSFT MT(MOD_RSFT, KC_2)
#define NV_RALT MT(MOD_RALT, KC_3)
#define NV_RCTL MT(MOD_RCTL, SE_PERC)


// Mac shortcuts
#define UNDO LCMD(KC_Z)
#define REDO LSG(KC_Z) // Left Shift + GUI (Cmd) + Z
#define CUT LCMD(KC_X)
#define COPY LCMD(KC_C)
#define PASTE LCMD(KC_V)

#define SKP_WDL LALT(KC_LEFT) // Skip word left
#define SKP_WDR LALT(KC_RIGHT) // Skip word right


// Macros
enum custom_keycodes {
    MS_DBL = SAFE_RANGE,
};
#define SFTLLCK LSFT_T(KC_0) // Locks layer on tap, shift on hold. KC_0 is arbitrary placeholder

// Process record user function for custom keycodes
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SFTLLCK:
            if (record->tap.count) {
                if (record->event.pressed) {
                    // Toggle the lock on the highest layer.
                    layer_lock_invert(get_highest_layer(layer_state));
                }
                return false;
            }
            break;
        case MS_DBL:
            if (record->event.pressed) {
                tap_code(MS_BTN1);
                wait_ms(50);
                tap_code(MS_BTN1);
            }
            return false;
        default:
            break;
    }
    return true;
}


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Esc   |   1  |   2  |   3  |   4  |   5  |                              |   6  |   7  |   8  |   9  |   0  |  Esc   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |   Å    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  |   Ö  |   Ä    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z/ |   X/ |   C/ |   V/ |   B  | [ {  |CapsWd|  | ESC  |  ] } |   N  |   M/ |   ,/ |   ./ |   -/ | RShift |
 * |        | LCTL | LALT | LSHFT|  LGUI|      |      |      |  |      |      |      | RGUI | RSFT | RALT | RCTL |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| MEH  |Space/| BSPC |   *  |  |   *  | Tab/ |Enter/| MEH  | Menu |
 *                        |      |      |Nav   |      |      |  |      | SYM  |Mouse |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * | MUTE | ____ | _____ | ____ | ____ |                                              | MUTE | ____ | _____ | ____ | ____ |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_QWERTY] = LAYOUT_elora_hlc(
     KC_ESC  ,   KC_1 ,  KC_2   ,  KC_3  ,   KC_4 ,   KC_5 ,                                         KC_6  ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_0 , KC_ESC ,
     KC_TAB  ,   KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,                                         KC_Y  ,   KC_U ,   KC_I ,   KC_O ,   KC_P , SE_ARNG,
     CTL_ESC ,   KC_A ,  KC_S   ,  KC_D  ,   KC_F ,   KC_G ,                                         KC_H  ,   KC_J ,   KC_K ,   KC_L , SE_ODIA, SE_ADIA,
     KC_LSFT , QH_LCTL,  QH_LALT, QH_LSFT, QH_LGUI,   KC_B , SE_LBRC,  CW_TOGG,    KC_ESC , SE_RBRC, KC_N  , QH_RGUI, QH_RSFT, QH_RALT, QH_RCTL, KC_RSFT,
                                  ADJUST , KC_MEH , SPC_NAV, KC_BSPC,  QK_REP ,    QK_REP , TAB_SYM, ENT_MS, KC_MEH , KC_APP,
     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),
/*
 * Base Layer: Gallium
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Esc   |   1  |   2  |   3  |   4  |   5  |                              |   6  |   7  |   8  |   9  |   0  |  Esc   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Tab   |   B  |   L  |   D  |   C  |   V  |                              |   J  |   Y  |   O  |   U  |   Ö  |   Å    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   N  |   R  |   T  |   S  |   G  |                              |   P  |   H  |   A  |   E  |   I  |   Ä    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   X/ |   Q/ |   M/ |   W/ |   Z  | [ {  |CapsWd|  | ESC  |  ] } |   K  |   F/ |   ,/ |   ./ |   -/ | RShift |
 * |        | LCTL | LALT | LSHFT|  LGUI|      |      |      |  |      |      |      | RGUI | RSFT | RALT | LCTL |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust|  MEH |Space/| BSPC |   *  |  |   *  | Tab  | Enter| MEH  | Menu |
 *                        |      |      |Nav   |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * | MUTE | ____ | _____ | ____ | ____ |                                              | MUTE | ____ | _____ | ____ | ____ |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_GALLIUM] = LAYOUT_elora_hlc(
     KC_ESC  , KC_1 ,  KC_2   ,  KC_3   , KC_4   , KC_5   ,                                       KC_6 , KC_7   ,  KC_8  , KC_9   , KC_0   , KC_ESC ,
     KC_TAB  , KC_B ,  KC_L   ,  KC_D   , KC_C   , KC_V   ,                                       KC_J,  KC_Y   ,  KC_O  , KC_U   , SE_ODIA, SE_ARNG,
     CTL_ESC , KC_N ,  KC_R   ,  KC_T   , KC_S   , KC_G   ,                                       KC_P,  KC_H   ,  KC_A  , KC_E   , KC_I   , SE_ADIA,
     KC_LSFT , GH_LCTL,GH_LALT,  GH_LSFT, GH_LGUI, KC_Z   , SE_LBRC , CW_TOGG,   KC_ESC, SE_RBRC, KC_K,  GH_RGUI, GH_RSFT, GH_RALT, GH_RCTL, KC_RSFT,
                                 ADJUST , KC_MEH , SPC_NAV, KC_BSPC , QK_REP ,   QK_REP, TAB_SYM, ENT_MS, KC_MEH, KC_APP,
     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                          KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),
/*
 * Base Layer: Gaming
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Esc   |   1  |   2  |   3  |   4  |   5  |                              |   6  |   7  |   8  |   9  |   0  |  Esc   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |   Å    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Ctrl/Esc|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  |   Ö  |   Ä    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  | [ {  |CapsWd|  | ESC  |  ] } |   N  |   M  |   ,  |   .  |   -  | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LGUI |Space | BSPC |   *  |  |   *  |  Tab | Enter| RGUI | Menu |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * | MUTE | ____ | _____ | ____ | ____ |                                              | MUTE | ____ | _____ | ____ | ____ |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_GAMING] = LAYOUT_elora_hlc(
     KC_ESC  ,   KC_1 ,  KC_2   ,  KC_3  ,   KC_4 ,   KC_5 ,                                        KC_6  ,   KC_7 ,   KC_8   ,   KC_9  ,   KC_0 , KC_ESC ,
     KC_TAB  ,   KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,                                        KC_Y  ,   KC_U ,   KC_I   ,   KC_O  ,   KC_P , SE_ARNG,
     CTL_ESC ,   KC_A ,  KC_S   ,  KC_D  ,   KC_F ,   KC_G ,                                        KC_H  ,   KC_J ,   KC_K   ,   KC_L  , SE_ODIA, SE_ADIA,
     KC_LSFT ,   KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , SE_LBRC, CW_TOGG,    KC_ESC , SE_RBRC, KC_N  ,   KC_M ,   KC_COMM,   KC_DOT, KC_MINS, KC_RSFT,
                                  ADJUST , KC_LGUI, KC_SPC, KC_BSPC ,  QK_REP,    QK_REP , KC_TAB , KC_ENT, KC_RGUI, KC_APP,
     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Nav Layer: Navigation, Num
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |   F1   |  F2  |  F3  |  F4  |  F5  |  F6  |                              |  F7  |  F8  |  F9  |  F10 |  F11 |   F12  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Cut   |Paste | Home |  ↑   | End  |      |                              |   +  |  7   |  8   |   9  |   *  |   :    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Copy  |  ⇤   |  ←   |  ↓   | →    |  ⇥   |                              |   =  |  4   |  5   |  6   |   /  |   <    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift/ |      | PgUp |      | PgDn |      |      |      |  |      |      |   -  |  1/  |  2/  |  3/  |  %/  |   >    |
 * | lock   |      |      |      |      |      |      |      |  |      |      |      | RGUI | RSFT | RALT | LCTL |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |   0  |  (   |  )   |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_NAV] = LAYOUT_elora_hlc(
      KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5   , KC_F6  ,                                          KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 ,
      CUT    , PASTE  , KC_HOME, KC_UP  , KC_END  , _______,                                          KC_PPLS, KC_7   , KC_8   , KC_9   , KC_PAST, SE_COLN,
      COPY   , SKP_WDL, KC_LEFT, KC_DOWN, KC_RIGHT, SKP_WDR,                                          KC_PEQL, KC_4   , KC_5   , KC_6   , KC_PSLS, SE_LABK,
      SFTLLCK, _______, KC_PGUP, _______, KC_PGDN , _______, _______, _______,      _______, _______, KC_MINS, NV_RGUI, NV_RSFT, NV_RALT, NV_RCTL, SE_RABK,
                                 _______, _______ , _______, _______, _______,      _______, _______, KC_0   , SE_LPRN, SE_RPRN,
     _______, _______       ,  _______, _______, _______ ,                                                            _______, _______, _______, _______, _______
    ),

/*
 * Mouse Layer
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Acl1  | Acl2 |      |  ↑   |      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Acl0  | ←Whl |  ←   |  ↓   | →    | →Whl |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift/ |      |      |      |      |      |Lock  |      |  |      |      |      |      |      |      |      | Shift/ |
 * | lock   |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      | lock   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |Double|Left  |Right |Middle|  |      |      |      |      |      |
 *                        |      |click |click |click |click |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_MS] = LAYOUT_elora_hlc(
      _______, _______, _______, _______, _______ , _______,                                          _______, _______, _______, _______, _______, _______,
      MS_ACL1, MS_ACL2, _______, MS_UP  , _______ , _______,                                          _______, _______, _______, _______, _______, _______,
      MS_ACL0, MS_WHLL, MS_LEFT, MS_DOWN, MS_RGHT,  MS_WHLR,                                          _______, _______, _______, _______, _______, _______,
      SFTLLCK, _______, _______, _______, _______ , _______, QK_LOCK, _______,      _______, _______, _______, _______, _______, _______, _______, SFTLLCK,
                                 _______, MS_DBL  , MS_BTN1, MS_BTN2, MS_BTN3,      _______, _______, _______, _______, _______,
     _______, _______,  _______, _______, _______ ,                                                            _______, _______, _______, _______, _______
    ),

/*
 * Sym Layer: Symbols
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |    `   |  1   |  2   |  3   |  4   |  5   |                              |   6  |  7   |  8   |  9   |  0   |   =    |
 * |--------+------+------+------+------+------|                             |------+------+------+------+------+--------|
 * |    ~   |  !   |  @   |  #   |  $   |  %   |                              |   ^  |  &   |  *   |  (   |  )   |   +    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift/ |   \  |  :   |  ;   |  -   |  [   |  {   |      |  |      |   }  |   ]  |  _   |  ,   |  .   |  /   |   ?    |
 * | lock   |      |      |      |      |      |      |      |  |      |      |      |      | RGUI | RSFT | RALT | LCTL   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_SYM] = LAYOUT_elora_hlc(
      _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
      KC_GRV ,   KC_1 ,   KC_2 ,   KC_3 ,   KC_4 ,   KC_5 ,                                       KC_6 ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_0 , KC_EQL ,
     KC_TILD , KC_EXLM,  KC_AT , KC_HASH,  KC_DLR, KC_PERC,                                     KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PLUS,
     KC_PIPE , KC_BSLS, KC_COLN, KC_SCLN, KC_MINS, KC_LBRC, KC_LCBR, _______, _______, KC_RCBR, KC_RBRC, KC_UNDS, KC_COMM,  KC_DOT, KC_SLSH, KC_QUES,
                                 _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
     _______, _______,  _______, _______, _______,                                                       _______, _______, _______, _______, _______
    ),

/*
 * Adjust Layer: Default layer settings, RGB
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  BOOT  |      |      |      |      |      |                              |      |      |      |      |      | BOOT   |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |QWERTY|      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |Gallium|     |      |                              | TOG  | SAI  | HUI  | VAI  | MOD  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |Gaming|      |      |      |      |  |      |      |      | SAD  | HUD  | VAD  | RMOD |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_ADJUST] = LAYOUT_elora_hlc(
      QK_BOOT, _______, _______, _______, _______, _______,                                    _______, _______, _______, _______, _______, QK_BOOT,
      _______, _______, _______, QWERTY , _______, _______,                                    _______, _______, _______, _______, _______, _______,
      RM_NEXT, _______, _______, GALLIUM, _______, _______,                                    RM_TOGG, RM_SATU, RM_HUEU, RM_VALU, RM_NEXT, _______,
      RM_PREV, _______, _______, GAMING , _______, _______,_______, _______, _______, _______, _______, RM_SATD, RM_HUED, RM_VALD, RM_PREV, _______,
                                 _______, _______, _______,_______, _______, _______, _______, _______, _______, _______,
     _______, _______,  _______, _______, _______,                                                      _______, _______, _______, _______, _______
    ),

// /*
//  * Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  * ,-----------------------------------.                                              ,-----------------------------------.
//  * |      |      |       |      |      |                                              |      |      |       |      |      |
//  * `-----------------------------------'                                              `-----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT_elora_hlc(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//      _______, _______,  _______, _______, _______,                                                       _______, _______, _______, _______, _______
//     ),
//
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(KC_PGUP, KC_PGDN),  ENCODER_CCW_CW(KC_PGUP, KC_PGDN)  }, // QWERTY
    [1] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  }, // Gallium
    [2] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  }, // Gaming
    [3] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  }, // Nav
    [4] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(MS_WHLU, MS_WHLD),  ENCODER_CCW_CW(_______, _______)  }, // Mouse
    [5] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  }, // Sym
    [6] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  } // Adjust
};
#endif

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // set base color on all keys
    hsv_t hsv = {0, 0, 100};
    rgb_t base_rgb = hsv_to_rgb(hsv);
    rgb_matrix_set_color_all(base_rgb.r, base_rgb.g, base_rgb.b);

    if (caps_word_enabled) {
        hsv.h = 0; // Set hue to red
        hsv.s = 255; // Max saturation
        rgb_t caps_word_rgb = hsv_to_rgb(hsv);
        RGB_MATRIX_INDICATOR_SET_COLOR(5, caps_word_rgb.r, caps_word_rgb.g, caps_word_rgb.b); // Underglow LED
        RGB_MATRIX_INDICATOR_SET_COLOR(11, caps_word_rgb.r, caps_word_rgb.g, caps_word_rgb.b); // Key
    }

    // set color on layer
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    switch (layer) {
        case L_NAV: 
            hsv.h = 4; // orange
            hsv.s = 255; // Max saturation
            hsv.v = 175;
            rgb_t nav_layer_rgb = hsv_to_rgb(hsv);
            uint8_t list_arrow_key_indices[5] = {27, 22, 21, 20, 8};
            for (uint8_t i = 0; i < 5; i++) {
                rgb_matrix_set_color(list_arrow_key_indices[i], nav_layer_rgb.r, nav_layer_rgb.g, nav_layer_rgb.b);
            }
            break;
        case L_ADJUST:
            hsv.h = 0;
            hsv.s = 255; // Max saturation
            hsv.v = 100;
            rgb_t adjust_layer_rgb = hsv_to_rgb(hsv);
            rgb_matrix_set_color(10, adjust_layer_rgb.r, adjust_layer_rgb.g, adjust_layer_rgb.b);
        default:
            // Do nothing for other layers
            break;
    }
 
    return false;
}

void caps_word_set_user(bool active) {
    if (active) {
        caps_word_enabled = true;
    } else {
        caps_word_enabled = false;
    }
}

bool shutdown_user(bool jump_to_bootloader) {
    
    if (jump_to_bootloader) {
        // red for bootloader
        hsv_t hsv = {0, 255, 100};
        rgb_t base_rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color_all(base_rgb.r, base_rgb.g, base_rgb.b);
    } else {
        // off for soft reset
        rgb_matrix_set_color_all(RGB_OFF);
    }
    // force flushing -- otherwise will never happen
    rgb_matrix_update_pwm_buffers();
    // false to not process kb level
    return false;
}