// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "_wait.h"
#include "action.h"
#include "action_layer.h"
#include "action_util.h"
#include "caps_word.h"
#include "color.h"
#include "community_modules.h"
#include "info_config.h"
#include "keyboard.h"
#include "keycodes.h"
#include "keymap_swedish.h"
#include "process_combo.h"
#include "sendstring_swedish.h"
#include "keymap_us.h"
#include "modifiers.h"
#include "quantum.h"
#include "quantum_keycodes.h"
#include "repeat_key.h"
#include "rgb_matrix.h"
#include "rgb_config.h"
#include "app_switcher.h"
#include "leader.h"
#include "select_word.h"
#include "transactions.h"
#include QMK_KEYBOARD_H

bool is_leader_active = false;
bool is_leader_changed = false;

enum layers {
    L_GALLIUM =0,
    L_QWERTY,
    L_GAMING,
    L_SYM,
    L_MS,
    L_NAV,
    L_ADJUST,
};

#define BKSLSH LSA(KC_7)
#define PERC LSFT(KC_5)
// Aliases for readability
#define QWERTY DF(L_QWERTY)
#define GALLIUM DF(L_GALLIUM)
#define GAMING DF(L_GAMING)

#define SYM MO(L_SYM)
#define SPC_NAV LT(L_NAV, KC_SPC)
#define TAB_MD LT(L_MS, KC_TAB)
#define ENT_SYM LT(L_SYM, KC_ENT)
#define ADJUST MO(L_ADJUST)

#define HYPR_ESC MT(MOD_LCTL | MOD_LALT | MOD_LSFT | MOD_LGUI, KC_ESC)
#define CTL_QUOT MT(MOD_RCTL, KC_QUOTE)
#define CTL_MINS MT(MOD_RCTL, KC_MINUS)
#define ALT_ENT MT(MOD_LALT, KC_ENT)
#define MEH_S MT(MOD_LCTL | MOD_LALT | MOD_LSFT, SE_S) // Meh or S


#define CW_WIN SGUI(KC_GRV)
#define CCW_WIN LGUI(KC_GRV)


// Define lower home row mods for GUI, shft, alt, ctrl

// QWERTY
// Left side
#define QH_LGUI MT(MOD_LGUI, SE_F)
#define QH_LSFT MT(MOD_LSFT, SE_D)
#define QH_LALT MT(MOD_LALT, SE_S)
#define QH_LCTL MT(MOD_LCTL, SE_A)
// Right side
#define QH_RGUI MT(MOD_RGUI, SE_J)
#define QH_RSFT MT(MOD_RSFT, SE_K)
#define QH_RALT MT(MOD_RALT, SE_L)
#define QH_RCTL MT(MOD_RCTL, SE_ODIA)

// Gallium
// Left side
#define GH_LGUI MT(MOD_LGUI, SE_C)
#define GH_LSFT MT(MOD_LSFT, SE_T)
#define GH_LALT MT(MOD_LALT, SE_R)
#define GH_LCTL MT(MOD_LCTL, SE_N)
// Right side
#define GH_RGUI MT(MOD_RGUI, SE_H)
#define GH_RSFT MT(MOD_RSFT, SE_A)
#define GH_RALT MT(MOD_RALT, SE_E)
#define GH_RCTL MT(MOD_RCTL, SE_I)

// Nav
// Right side
#define NV_RGUI MT(MOD_RGUI, KC_4)
#define NV_RSFT MT(MOD_RSFT, KC_5)
#define NV_RALT MT(MOD_RALT, KC_6)
#define NV_RCTL MT(MOD_RCTL, BKSLSH)


// Mac shortcuts
#define UNDO LCMD(KC_Z)
#define REDO LSG(KC_Z) // Left Shift + GUI (Cmd) + Z
#define CUT LCMD(KC_X)
#define COPY LCMD(KC_C)
#define PASTE LCMD(KC_V)
#define PREFIX LCTL(KC_SPC)

#define SKP_WDL LALT(KC_LEFT) // Skip word left
#define SKP_WDR LALT(KC_RIGHT) // Skip word right

// VS Code Shortcuts
#define STP_IN LSA(LGUI(KC_H))  
#define STP_BK LSG(KC_H)  
#define STP_FWD LSG(KC_A)
#define PEEK LSA(LCTL(KC_H))


// Macros
enum custom_keycodes {
    MS_DBL = SAFE_RANGE,
    APP_KNB,
    APP_CW,
    APP_CCW,
    DEL_BWD,
    DEL_FWD,
    LEAD,
    LARCANE,
    RARCANE,
};
#define SFTLLCK LSFT_T(KC_0) // Locks layer on tap, shift on hold. KC_0 is arbitrary placeholder
#define MEH_LD MT(MOD_LCTL | MOD_LALT | MOD_LSFT, LEAD) // Meh or Leader

const uint16_t PROGMEM arng_combo[] = {GH_RSFT, SE_O, COMBO_END}; // Å
const uint16_t PROGMEM adia_combo[] = {GH_RGUI, SE_Y, COMBO_END}; // Ä
const uint16_t PROGMEM odia_combo[] = {GH_RSFT, SE_DOT, COMBO_END}; // Ö


combo_t key_combos[] = {
    COMBO(arng_combo, SE_ARNG),
    COMBO(adia_combo, SE_ADIA),
    COMBO(odia_combo, SE_ODIA),
};

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
 * |Ctrl/Esc|   A/ |   S/ |   D/ |   F/ |   G  |                              |   H  |   J/ |   K/ |   L/ |   Ö/ |   Ä    |
 * |        | LCTL | LALT | LSHFT|  LGUI|      |                              |      | RGUI | RSFT | RALT | RCTL |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  | [ {  |CapsWd|  | ESC  |  ] } |   N  |   M  |   ,  |   .  |   -  | RShift |
 * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| BSPC |Space/| MEH  |   *  |  |   *  | MEH  |Enter/| TAB/ | Menu |
 *                        |      |      |Nav   |      |      |  |      |      |Mouse | SYM  |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * | MUTE | ____ | _____ | ____ | ____ |                                              | MUTE | ____ | _____ | ____ | ____ |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_QWERTY] = LAYOUT_elora_hlc(
     KC_ESC  ,   KC_1 ,  KC_2   ,  KC_3  ,   KC_4 ,   KC_5 ,                                         KC_6   ,   KC_7 ,   KC_8 ,   KC_9 ,   KC_0 , KC_ESC ,
     KC_TAB  ,   KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,                                         KC_Y   ,   KC_U ,   KC_I ,   KC_O ,   KC_P , SE_ARNG,
     HYPR_ESC, QH_LCTL,  QH_LALT,QH_LSFT , QH_LGUI,   KC_G ,                                         KC_H   ,QH_RGUI , QH_RSFT, QH_RALT, QH_RCTL, SE_ADIA,
     KC_LSFT ,   KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B , SE_LBRC,  CW_TOGG,    KC_ESC , SE_RBRC, KC_N   ,   KC_M , SE_COMM, SE_DOT , SE_MINS, KC_RSFT,
                                  ADJUST , KC_BSPC, SPC_NAV, KC_MEH ,  LARCANE,    RARCANE, KC_MEH , ENT_SYM, TAB_MD, KC_APP,
     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),
/*
 * Base Layer: Gallium
 * 
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |   F1   |  F2  |  F3  |  F4  |  F5  |  F6  |                              |  F7  |  F8  |  F9  |  F10 |  F11 |   F12  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * | PREFIX |   B  |   L  |   D  |   W  |   V  |                              |   J  |   Y  |   O  |   U  |   !  |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Hypr/Esc|   N/ |   R/ |   T/ |   C/ |   G  |                              |   K  |   H/ |   A/ |   E/ |   I/ |Hypr/Esc|
 * |        | LCTL | LALT | LSHFT|  LGUI|      |                              |      | RGUI | RSFT | RALT | RCTL |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   X  |   Q  |   M  |   P  |   Z  | BSPC |CapsWd|  | DEL  |Leader|PREFIX|   F  |   ,  |   .  |   -  | RShift |
 * |        |      |      |      |      |      |      |      |  |      |/MEH  |      |      |      |      |      |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| CUT  | S/   |Space/|   *  |  |   *  |Enter/| TAB/ | UNDO | REDO |
 *                        |      |      | MEH  |Nav   |      |  |      | SYM  |MOUSE |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * | MUTE | ____ | _____ | ____ | ____ |                                              | MUTE | ____ | _____ | ____ | ____ |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_GALLIUM] = LAYOUT_elora_hlc(
     KC_F1   , KC_F2  , KC_F3  ,  KC_F4  , KC_F5 , KC_F6  ,                                       KC_F7  ,  KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 ,
     PREFIX  , SE_B   ,  SE_L  ,  SE_D   , SE_W  , SE_V   ,                                       SE_J   ,  SE_Y   , SE_O   , SE_U   , SE_EXLM, KC_NO  ,
     HYPR_ESC, GH_LCTL,GH_LALT,  GH_LSFT, GH_LGUI, KC_G   ,                                       SE_K   ,  GH_RGUI, GH_RSFT, GH_RALT, GH_RCTL, HYPR_ESC,
     KC_LSFT , SE_X ,  SE_Q   ,  SE_M   , SE_P   , SE_Z    , KC_BSPC , CW_TOGG,   KC_DEL , MEH_LD ,PREFIX,  SE_F   , KC_DOT , KC_COMM , SE_MINS, KC_RSFT,
                                 ADJUST , CUT    , MEH_S  , SPC_NAV , LARCANE,   RARCANE, ENT_SYM, TAB_MD,  UNDO   , REDO   ,
     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                          KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),
/*
 * Base Layer: Gaming
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  Esc   |   1  |   2  |   3  |   4  |   5  |                              |   6  |   7  |   8  |   9  |   0  |  RAlt  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  Tab   |   Q  |   W  |   E  |   R  |   T  |                              |   Y  |   U  |   I  |   O  |   P  |   Å    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |Hypr/Esc|   A  |   S  |   D  |   F  |   G  |                              |   H  |   J  |   K  |   L  |   Ö  |   Ä    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | LShift |   Z  |   X  |   C  |   V  |   B  | [ {  |CapsWd|  | ESC  |  ] } |   N  |   M  |   ,  |   .  |   -  | RShift |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |Adjust| LGUI |Space | BSPC |   *  |  |   *  | RGUI | Enter|  Tab | Menu |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * | MUTE | ____ | _____ | ____ | ____ |                                              | MUTE | ____ | _____ | ____ | ____ |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_GAMING] = LAYOUT_elora_hlc(
     KC_ESC  ,   KC_1 ,  KC_2   ,  KC_3  ,   KC_4 ,   KC_5 ,                                        KC_6  ,   KC_7 ,   KC_8   ,   KC_9  ,   KC_0 , KC_RALT,
     KC_TAB  ,   KC_Q ,  KC_W   ,  KC_E  ,   KC_R ,   KC_T ,                                        KC_Y  ,   KC_U ,   KC_I   ,   KC_O  ,   KC_P , SE_ARNG,
     HYPR_ESC,   KC_A ,  KC_S   ,  KC_D  ,   KC_F ,   KC_G ,                                        KC_H  ,   KC_J ,   KC_K   ,   KC_L  , SE_ODIA, SE_ADIA,
     KC_LSFT ,   KC_Z ,  KC_X   ,  KC_C  ,   KC_V ,   KC_B ,  SE_LBRC, CW_TOGG,    KC_ESC , SE_RBRC, KC_N  ,   KC_M ,   KC_COMM,   KC_DOT, KC_MINS, KC_RSFT,
                                  ADJUST , KC_BSPC,  KC_SPC,  KC_LGUI,  QK_REP,    QK_REP , KC_RGUI, KC_ENT,  KC_TAB, KC_APP,
     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                                KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Sym Layer: Symbols
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  <   |  >   |  '   |  "   |  -   |                              |   ?  |  [   |  ]   |  &   |      |   %    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |   !  |  *   |  /   |  =   |  |   |                              |   ;  |  (   |  )   |   :  |  ~   |   ^    |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift/ |   \  |  +   |  \   |  _   |  @   |  $   |      |  |      |      |   #  |  {   |  }   |  .   |back  | Shift/ |
 * | lock   |      |      |      |      |      |      |      |  |      |      |      |      |      |      |tick  | lock   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_SYM] = LAYOUT_elora_hlc(
      _______, _______, _______, _______, _______, _______,                                         _______, _______ , _______  , _______, _______, _______,
      KC_NO  , KC_GRV,S(KC_GRV),SE_QUOT, SE_DQUO, SE_MINS,                                          SE_QUES, SE_LBRC  ,SE_RBRC  , SE_AMPR, _______, SE_PERC,
      KC_NO  , SE_EXLM, SE_ASTR, SE_SLSH, SE_EQL , A(KC_7),                                         SE_SCLN, SE_LPRN  ,SE_RPRN  , SE_COLN, SE_TILD, SE_CIRC,
      SFTLLCK, KC_NO  , SE_PLUS, BKSLSH , SE_UNDS, SE_AT  , SE_DLR , KC_NO ,     _______, _______,  SE_HASH, LSA(KC_8),LSA(KC_9), KC_DOT,  SE_ACUT, SFTLLCK,
                                _______, _______, _______, _______, _______,     _______, _______,  _______, _______, _______,
     _______, _______,  _______, _______, _______,                                                               _______, _______, _______, _______, _______
    ),

    /*
 * Nav Layer: Navigation, Num
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |   F1   |  F2  |  F3  |  F4  |  F5  |  F6  |                              |  F7  |  F8  |  F9  |  F10 |  F11 |   F12  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | Paste| PgUp |  ↑   | PgDn | Redo |                              |   +  |  7   |  8   |   9  |   *  |   :    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | Copy |  ←   |  ↓   | →    | Undo |                              |   =  |  4/  |  5/  |  6/  |   /  |        |
 * |        |      |      |      |      |      |                              |      | RGUI | RSFT | RALT | LCTL |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift/ | Cut  | Home |      | End  |      |      |      |  | PEEK |STP_FW|   -  |  1/  |  2/  |  3/  |  %/  | Shift/ |
 * | lock   |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      | lock   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |STP_IN|STP_BK|   0  |  <   |  >   |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_NAV] = LAYOUT_elora_hlc(
      KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5   , KC_F6  ,                                          KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 ,
      _______, PASTE  , KC_PGUP, KC_UP  , KC_PGDN , REDO   ,                                          SE_PLUS, KC_7   , KC_8   , KC_9   , SE_ASTR, SE_COLN,
      _______, COPY   , KC_LEFT, KC_DOWN, KC_RIGHT, UNDO   ,                                          SE_EQL , NV_RGUI, NV_RSFT, NV_RALT, NV_RCTL, KC_NO,
      SFTLLCK, CUT    , KC_HOME, KC_NO  , KC_END  , _______, _______, _______,     PEEK   , STP_FWD,  SE_MINS, KC_1   , KC_2   , KC_3   , PERC   , SFTLLCK,
                                 _______, _______ , _______, _______, _______,     STP_IN , STP_BK ,  KC_0   , KC_GRV , S(KC_GRV),
     _______, _______       ,  _______, _______, _______ ,                                                            APP_KNB, _______, _______, _______, _______
    ),

/*
 * Mouse Layer
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      | ↑Whl |  ↑   | ↓Whl |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | ←Whl |  ←   |  ↓   | →    | →Whl |                              |      | Acl0 | Acl1 | Acl2 |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift/ |      |      |      |      |      |Lock  |      |  |      |      |      |      |      |      |      | Shift/ |
 * | lock   |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      | lock   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |Middle|Double|Left  |Right |  |      |      |      |      |      |
 *                        |      |click |click |click |click |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_MS] = LAYOUT_elora_hlc( 
      KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO   ,                                          _______, _______, _______, _______, _______, _______,
      KC_NO  , KC_NO  , MS_WHLU, MS_UP  , MS_WHLD, KC_NO   ,                                          _______, KC_NO  , KC_NO  , KC_NO  , _______, _______,
      KC_NO  , MS_WHLL, MS_LEFT, MS_DOWN, MS_RGHT,  MS_WHLR,                                          _______, MS_ACL0, MS_ACL1, MS_ACL2, _______, _______,
      SFTLLCK, KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO   , QK_LOCK, _______,      _______, _______, _______, KC_NO  , KC_NO  , KC_NO  , _______, SFTLLCK,
                                 _______, MS_BTN3, MS_DBL  , MS_BTN1, MS_BTN2,      _______, _______, _______, _______, _______,
     _______, _______,  _______, _______, _______,                                                            _______, _______, _______, _______, _______
    ),
/*
 * Adjust Layer: Default layer settings, RGB
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |  BOOT  |      |      |      |      |      |                              |      |      |      |      |      | PWR    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |QWERTY|      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |Gallium|     |      |                              | TOG  | SAI  | HUI  | VAI  | MOD  |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |        |      |      |Gaming|      |      |      |      |  |      |      |      | SAD  | HUD  | VAD  | RMOD |        |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |CG_   |      |      |
 *                        |      |      |      |      |      |  |      |      |TOGG  |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_ADJUST] = LAYOUT_elora_hlc(
      QK_BOOT, EE_CLR , _______, _______, _______, _______,                                    _______, _______, _______, _______, _______, KC_PWR ,
      _______, _______, _______, QWERTY , _______, _______,                                    _______, _______, _______, _______, _______, _______,
      RM_NEXT, _______, _______, GALLIUM, _______, _______,                                    RM_TOGG, RM_SATU, RM_HUEU, RM_VALU, RM_NEXT, _______,
      RM_PREV, _______, _______, GAMING , _______, _______,_______, _______, _______, _______, _______, RM_SATD, RM_HUED, RM_VALD, RM_PREV, _______,
                                 _______, _______, _______,_______, _______, _______, _______, CG_TOGG, _______, _______,
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
    [L_GALLIUM] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(CW_WIN, CCW_WIN),  ENCODER_CCW_CW(APP_CW, APP_CCW)  },
    [L_QWERTY] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [L_GAMING] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [L_NAV] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(CW_WIN, CCW_WIN)  },
    [L_MS] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(MS_WHLU, MS_WHLD),  ENCODER_CCW_CW(_______, _______)  },
    [L_SYM] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  },
    [L_ADJUST] = { ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______)  }
};
#endif


bool remember_last_key_user(uint16_t keycode, keyrecord_t* record,
                            uint8_t* remembered_mods) {
    switch (keycode) {
        case CW_TOGG:
        case KC_ESC:
        case LARCANE:
        case RARCANE:
            return false;  // Magic keys will ignore the above keycodes.
    }
    return true;  // Other keys can be repeated.
}

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
  set_last_keycode(repeat_keycode); // 2024-03-09 Disabled sending of string for mag-rep / rep-mag consistency.
 
  // If Caps Word is on, restore the mods.
  if (is_caps_word_on()) {
    set_mods(saved_mods);
  }
}
static void process_right_arcane(uint16_t keycode, uint8_t mods) { // RARCANE definitions
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

        // default case, repeat
        default: tap_code16(keycode);
        
    }
}
static void process_left_arcane(uint16_t keycode, uint8_t mods) { // LARCANE definitions
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

        // default case, repeat
        default: tap_code16(keycode);
        
    }
}


void double_tap(uint16_t keycode) {
    tap_code16(keycode);
    wait_ms(50);
    tap_code16(keycode);
}

void delete_word_backwards(void) {
    register_code(KC_LGUI);
    register_code(KC_LSFT);
    tap_code(KC_LEFT);
    unregister_code(KC_LGUI);
    unregister_code(KC_LSFT);
    tap_code(KC_BSPC);
}

void delete_word_forwards(void) {
    register_code(KC_LGUI);
    register_code(KC_LSFT);
    tap_code(KC_RIGHT);
    unregister_code(KC_LGUI);
    unregister_code(KC_LSFT);
    tap_code(KC_BSPC);
}   

// Combo key handling
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Handle Arcane keys
     if (record->event.pressed) {
        switch (keycode) {
            case LARCANE: { process_left_arcane(get_last_keycode(), get_last_mods()); } return false;
            case RARCANE: { process_right_arcane(get_last_keycode(), get_last_mods()); } return false;
        }
    }
    
    // Handle knob
     if (record->event.pressed) {
        switch (keycode){
            case APP_CW:
                app_switch(KC_LEFT);
                return false;
            case APP_CCW:
                app_switch(KC_RIGHT);
                return false;
            case APP_KNB:
                end_app_switcher_with_selection();
                return false;
        }
    }

    switch (keycode) {
        case MEH_LD:
        case KC_SCROLL_LOCK:
            if (record->tap.count && record->event.pressed) {
                leader_start();
                return false;
            }
            break;
        case DEL_BWD:
            if (record->event.pressed) {
                delete_word_backwards();
            }
            return false;
        case DEL_FWD:
            if (record->event.pressed) {
                delete_word_forwards();
            }
            return false;
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
                double_tap(MS_BTN1);
            }
            return false;
        default:
            break;
    }
    return true;
}

void leader_start_user(void){
    is_leader_active = true;
    is_leader_changed = true;
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
    }else if (leader_sequence_one_key(KC_F)) {
        // send f"" for Python f-strings and place cursor in between
        tap_code(KC_F);
        double_tap(SE_DQUO);
        tap_code(KC_LEFT);
    }else if (leader_sequence_two_keys(KC_E, KC_Q)) {
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
        // -> : (arrow)
        tap_code16(SE_MINS);
        tap_code16(S(KC_GRV));
        tap_code16(KC_SPACE);
        tap_code16(SE_COLN);
        tap_code16(KC_LEFT);
    } else if (leader_sequence_two_keys(KC_R, KC_BSPC)) {
        delete_word_backwards();
        set_last_keycode(DEL_BWD);
    } else if (leader_sequence_two_keys(KC_C, KC_BSPC)) {
        delete_word_forwards();
        set_last_keycode(DEL_FWD);
    } else if (leader_sequence_two_keys(KC_C, KC_DOT)) {
        // "code ."
        SEND_STRING("code .");
        tap_code16(KC_ENT);
    } else if (leader_sequence_two_keys(SE_E, SE_P)){
        // exit Python PDB
        SEND_STRING("import os");
        tap_code16(KC_ENT);
        SEND_STRING("os._exit(0)");
        tap_code16(KC_ENT);
    } else if (leader_sequence_two_keys(KC_S, KC_W)) {
        // select word
        select_word_tap('W');
        set_last_keycode(SELWORD);
    } else if (leader_sequence_two_keys(KC_S, KC_B)) {
        // select word backwards
        select_word_tap('B');
        set_last_keycode(SELWBAK);
    } else if (leader_sequence_two_keys(KC_S, KC_L)) {
        // select word line
        select_word_tap('L');
        set_last_keycode(SELLINE);
    }

    is_leader_active = false;
    is_leader_changed = true;
}


// Timeout handling
void matrix_scan_user(void) {
    if (app_switcher_active && timer_elapsed(app_switcher_timer) > APP_SWITCHER_TIMEOUT) {
        end_app_switcher_with_selection();
    }
}

// RGB Profiles - Functions moved to rgb_config.c
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // set base color on all keys
    set_layer_rgb_by_configs(&base_layer_config);

    if (is_caps_word_on()) {
        set_layer_rgb_by_configs(&caps_word_layer_config);
    }

    if (is_leader_active) {
        set_layer_rgb_by_configs(&leader_layer_config);
    }

    // set color on all active layers (from lowest to highest priority)
    layer_state_t active_layers = layer_state | default_layer_state;
    
    // Check each layer in order (higher layers will override lower ones)
    for (uint8_t layer = 0; layer < MAX_LAYER; layer++) {
        if (IS_LAYER_ON_STATE(active_layers, layer)) {
            switch (layer) {
                case L_NAV: 
                    set_layer_rgb_by_configs(&nav_layer_config);
                    break;
                case L_MS:
                    set_layer_rgb_by_configs(&mouse_layer_config);
                    break;
                case L_SYM:
                    set_layer_rgb_by_configs(&symbol_layer_config);
                    break;
                case L_ADJUST:
                    set_layer_rgb_by_configs(&adjust_layer_config);
                    break;
                default:
                    // Do nothing for other layers
                    break;
            }
        }
    }
 
    return false;
}

// rgb matrix shutdown user function
bool shutdown_user(bool jump_to_bootloader) {
    
    if (jump_to_bootloader) {
        // red for bootloader
        set_layer_rgb_by_configs(&bootloader_config);
    } else {
        // off for soft reset
        rgb_matrix_set_color_all(RGB_OFF);
    }
    // force flushing -- otherwise will never happen
    rgb_matrix_update_pwm_buffers();
    // false to not process kb level
    return false;
}

bool get_speculative_hold(uint16_t keycode, keyrecord_t* record) {
  return (QK_MOD_TAP_GET_MODS(keycode) & (MOD_LALT | MOD_LGUI)) == 0;
}


// Caps Word behavior
bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        // Keycodes that continue Caps Word, with shift applied.
        case KC_A ... KC_Z:
        case SE_ADIA:
        case SE_ODIA:
        case SE_ARNG:
        case KC_MINS:
        case SE_MINS:
            add_weak_mods(MOD_BIT(KC_LSFT));  // Apply shift to next key.
            return true;

        // Keycodes that continue Caps Word, without shifting.
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
        case KC_UNDS:
        case KC_UP:
        case KC_DOWN:
        case KC_LEFT:
        case KC_RIGHT:
            return true;

        default:
            return false;  // Deactivate Caps Word.
    }
}



// Chorded hold behavior, overrides default to allow some one-handed chords.
bool get_chordal_hold(uint16_t tap_hold_keycode, keyrecord_t* tap_hold_record,
                      uint16_t other_keycode, keyrecord_t* other_record) {
    // Exceptionally allow some one-handed chords for hotkeys.
    switch (tap_hold_keycode) {  
        case SPC_NAV: 
            if (other_keycode == KC_D || other_keycode == GH_LSFT ||
                other_keycode == GH_LALT || other_keycode == GH_LGUI ||
                other_keycode == KC_B || other_keycode == KC_N || other_keycode == KC_X) {
                return true;
            }
            break;
        case HYPR_ESC:
            if (other_keycode == KC_UP || other_keycode == KC_DOWN ||
                other_keycode == KC_LEFT || other_keycode == KC_RIGHT) {
                return true;
            }
    }
    // Otherwise defer to the opposite hands rule.
    return get_chordal_hold_default(tap_hold_record, other_record);
}