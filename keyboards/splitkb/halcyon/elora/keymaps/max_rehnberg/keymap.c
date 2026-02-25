// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "keycodes.h"
#include QMK_KEYBOARD_H
#include <math.h>
#include "keymap_swedish.h"
#include "gallium_defs.h"
#include "sendstring_swedish.h"
#include "rgb_config.h"
#include "app_switcher.h"
#include "arcane.h"
#include "leader.h"
#include "sentence_case.h"
#include "transactions.h"
#include "xcase.h"
#include "split_util.h"
#include <string.h>

static bool    is_leader_active            = false;
static bool    is_leader_changed           = false;
static uint8_t sentence_case_on_synced     = 0;
static uint8_t sentence_case_primed_synced = 0;
static uint8_t leader_active_synced        = 0;

enum layers {
    L_GALLIUM = 0,
    L_SYM,
    L_NUM,
    L_MS,
    L_NAV,
    L_ADJUST,
};

#define PREFIX LCMD(KC_SPC)
#define BKSLSH LSA(KC_7)
#define PERC LSFT(KC_5)
// Aliases for readability
#define GALLIUM DF(L_GALLIUM)

#define MOUSE LT(L_MS, KC_NO)
#define SPC_NUM LT(L_NUM, KC_SPC)
#define TAB_NAV LT(L_NAV, KC_TAB)
#define ENT_SYM LT(L_SYM, KC_ENT)
#define ADJUST MO(L_ADJUST)
#define OSFT OSM(MOD_LSFT) // One-shot Shift

#define HYPR_ESC MT(MOD_LCTL | MOD_LALT | MOD_LSFT | MOD_LGUI, KC_ESC)
#define BSPC_MEH KC_BSPC // MT(MOD_LCTL | MOD_LALT | MOD_LSFT, KC_BSPC) // Meh or Backspace

#define CW_WIN SGUI(KC_GRV)
#define CCW_WIN LGUI(KC_GRV)

// Mac shortcuts
#define UNDO LCMD(KC_Z)
#define REDO LSG(KC_Z) // Left Shift + GUI (Cmd) + Z
#define CUT LCMD(KC_X)
#define COPY LCMD(KC_C)
#define PASTE LCMD(KC_V)

// Macros
enum custom_keycodes { MS_DBL = SAFE_RANGE, APP_KNB, APP_CW, APP_CCW, LEAD, LARCANE, RARCANE };
#define SCASE SENTENCE_CASE_TOGGLE                      // Alias for built-in Sentence Case toggle
#define SFTLLCK LSFT_T(KC_0)                            // Locks layer on tap, shift on hold. KC_0 is arbitrary placeholder
#define MEH_LD MT(MOD_LCTL | MOD_LALT | MOD_LSFT, LEAD) // Meh or Leader

const uint16_t PROGMEM arng_combo[] = {GH_RHM3, SE_O, COMBO_END};   // Å
const uint16_t PROGMEM adia_combo[] = {GH_RHM2, SE_U, COMBO_END};   // Ä
const uint16_t PROGMEM odia_combo[] = {GH_RHM3, SE_DOT, COMBO_END}; // Ö

combo_t key_combos[] = {
    COMBO(arng_combo, SE_ARNG),
    COMBO(adia_combo, SE_ADIA),
    COMBO(odia_combo, SE_ODIA),
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: Gallium
 *
 * ,-------------------------------------------.                               ,-------------------------------------------.
 * |   F1   |  F2  |  F3  |  F4  |  F5  |  F6  |                               |  F7  |  F8  |  F9  |  F10 |  F11 |   F12  |
 * |--------+------+------+------+------+------|                               |------+------+------+------+------+--------|
 * |    \   |   B  |   L  |   D  |   W  |   V  |                               |   J  |   Y  |   O  |   U  |   !  |   /    |
 * |--------+------+------+------+------+------|                               |------+------+------+------+------+--------|
 * |Hypr/Esc|   N/ |   R/ |   T/ |   C/ |   G  |                               |   K  |   H/ |   A/ |   E/ |   I/ |Hypr/Esc|
 * |        | LCTL | LALT | LSHFT|  LGUI|      |                               |      | RGUI | RSFT | RALT | RCTL |        |
 * |--------+------+------+------+------+------+--------------.  ,--------------+-----+------+------+------+------+--------|
 * |CapsWord|   X  |   Q  |   M  |   P  |   Z  | BSPC/|OS(SFT)|  |OS(SFT)|Leader|  :   |   F  |   .  |   ,  |   -  |  DEL  |
 * |        |      |      |      |      |      | MEH  |       |  |       |/MEH  |      |      |      |      |      |       |
 * `----------------------+------+------+------+------+-------|  |------+------+------+------+------+----------------------'
 *                        |Adjust|PREFIX|   S  |Space/|   *   |  |   *   |Enter/| TAB/ |PREFIX|Sentence |
 *                        |      |      |      |NUM   |       |  |       | SYM  | NAV  |MOUSE |Case     |
 *                        `-----------------------------------'  `-------------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * | MUTE | ____ | _____ | ____ | ____ |                                              | MUTE | ____ | _____ | ____ | ____ |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_GALLIUM] = LAYOUT_elora_hlc(
     KC_F1   , KC_F2  , KC_F3  ,  KC_F4  , KC_F5 , KC_F6  ,                                       KC_F7  ,  KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 ,
     BKSLSH  , SE_B   ,  SE_L  ,  SE_D   , SE_W  , SE_V   ,                                       SE_J   ,  SE_Y   , SE_O   , SE_U   , SE_EXLM, SE_SLSH ,
     HYPR_ESC, GH_LHM1,GH_LHM2,  GH_LHM3, GH_LHM4, KC_G   ,                                       SE_K   ,  GH_RHM4, GH_RHM3, GH_RHM2, GH_RHM1, HYPR_ESC,
     CW_TOGG , SE_X ,  SE_Q   ,  SE_M   , SE_P   , SE_Z   , BSPC_MEH, OSFT,      OSFT   , MEH_LD,SE_COLN,  SE_F   , KC_DOT , KC_COMM, SE_MINS,  KC_DEL  ,
                                 ADJUST , PREFIX , SE_S   , SPC_NUM , LARCANE,   RARCANE, ENT_SYM, TAB_NAV,  MOUSE , SCASE  ,
     KC_MUTE, KC_NO,  KC_NO, KC_NO, KC_NO,                                                          KC_MUTE, KC_NO, KC_NO, KC_NO, KC_NO
    ),

/*
 * Sym Layer: Symbols
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |  <   |  >   |  '   |  "   |  |   |                              |   ?  |  )   |  ]   |  }   |  !   |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |  *   |  +   |  =   |  _   |                              |   $  |  (   |  [   |  {   |  ~   |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift/ |      |      |      |  &   |  @   |      |      |  |      |      |   #  |  %   |  ^   |  ;   |back  | Shift/ |
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
      _______, _______, _______, _______, _______, _______,                                         _______, _______ ,_______, _______  , _______, _______,
      KC_NO  , KC_GRV ,S(KC_GRV),SE_QUOT, SE_DQUO, A(KC_7),                                         SE_QUES, SE_RPRN ,SE_RBRC, LSA(KC_9), _______, KC_NO  ,
      KC_NO  , KC_NO  , SE_ASTR ,SE_PLUS, SE_EQL , SE_UNDS,                                         SE_DLR , SE_LPRN ,SE_LBRC, LSA(KC_8), SE_TILD, KC_NO  ,
      SFTLLCK, KC_NO  , KC_NO   , KC_NO , SE_AMPR, SE_AT  , SE_DLR , KC_NO ,     _______, _______,  SE_HASH, PERC    ,SE_CIRC, SE_SCLN  , SE_ACUT, SFTLLCK,
                                _______, _______, _______, _______, _______,     _______, _______,  _______, _______, _______,
     _______, _______,  _______, _______, _______,                                                               _______, _______, _______, _______, _______
    ),

    /*
 * Nav Layer: Navigation
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |   F1   |  F2  |  F3  |  F4  |  F5  |  F6  |                              |  F7  |  F8  |  F9  |  F10 |  F11 |   F12  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | Paste| PgUp |  ↑   | PgDn | Redo |                              |   +  |  7   |  8   |   9  |   *  |   :    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | Copy |  ←   |  ↓   | →    | Undo |                              |   =  |  4/  |  5/  |  6/  |   /  |        |
 * |        |      |      |      |      |      |                              |      | RGUI | RSFT | RALT | LCTL |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift/ | Cut  | Home |      | End  |      |      |      |  |      |      |   -  |  1/  |  2/  |  3/  |  %/  | Shift/ |
 * | lock   |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      | lock   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |      |   0  |  <   |  >   |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_NAV] = LAYOUT_elora_hlc(
      KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5   , KC_F6  ,                                          KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 ,
      _______, PASTE  , KC_PGUP, KC_UP  , KC_PGDN , REDO   ,                                          SE_PLUS, KC_7   , KC_8   , KC_9   , SE_ASTR, SE_COLN,
      HYPR_ESC, COPY  , KC_LEFT, KC_DOWN, KC_RIGHT, UNDO   ,                                          SE_EQL , NV_RHM4, NV_RHM3, NV_RHM2, NV_RHM1, HYPR_ESC,
      SFTLLCK, CUT    , KC_HOME, KC_NO  , KC_END  , _______, _______,   _______,_______,  _______,  SE_MINS, KC_1   , KC_2   , KC_3   , PERC   , SFTLLCK,
                                _______, _______ , _______, _______, _______,   _______,  _______,  KC_0, KC_GRV , S(KC_GRV),
     _______, _______       ,  _______, _______, _______ ,                                                            APP_KNB, _______, _______, _______, _______
    ),

    /*
 * Nav Layer: Numpad
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |   F1   |  F2  |  F3  |  F4  |  F5  |  F6  |                              |  F7  |  F8  |  F9  |  F10 |  F11 |   F12  |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        |      |      |      |      |      |                              |   +  |  7   |  8   |   9  |   *  |        |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |        | CTRL | OPT  |Shift | GUI  |      |                              |   =  |  4/  |  5/  |  6/  |   /  |        |
 * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * | Shift/ |      |      |      |      |      |      |      |  |      |      |   -  |  1/  |  2/  |  3/  |  %/  | Shift/ |
 * | lock   |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      | lock   |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |      |      |      |      |      |  |      |   0  |      |  <   |  >   |
 *                        |      |      |      |      |      |  |      |      |      |      |      |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [L_NUM] = LAYOUT_elora_hlc(
      KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  , KC_F6  ,                                          KC_F7  , KC_F8  , KC_F9  , KC_F10 , KC_F11 , KC_F12 ,
      _______, KC_NO  , KC_NO  , KC_NO  , KC_NO  , KC_NO  ,                                          SE_PLUS, KC_7   , KC_8   , KC_9   , SE_ASTR, KC_NO,
      _______, KC_HM1, KC_HM2, KC_HM3, KC_HM4, KC_NO  ,                                          SE_EQL , KC_4   , KC_5   , KC_6   , SE_SLSH, KC_NO,
      SFTLLCK, KC_NO  , KC_NO  , KC_NO  , KC_NO  , _______, _______,   _______,_______,  _______,    SE_MINS, KC_1   , KC_2   , KC_3   , PERC   , SFTLLCK,
                                _______, _______ , _______, _______, _______,   _______, _______,    KC_0   , KC_GRV , S(KC_GRV),
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
 * |        |      |      |      |      |      |                              |      |      |      |      |      | PWR    |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |  BOOT  |      |      |QWERTY|      |      |                              |      |      |      |      |      |        |
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
      _______, _______, _______, _______, _______, _______,                                       _______, _______, _______, _______, _______, KC_PWR ,
      QK_BOOT, _______, _______, _______, _______, _______,                                       _______, _______, _______, _______, _______, _______,
      RM_NEXT, _______, _______, _______, _______, _______,                                       RM_TOGG, RM_SATU, RM_HUEU, RM_VALU, RM_NEXT, _______,
      RM_PREV, _______, _______, _______, _______, _______,_______, _______, _______, _______, _______, RM_SATD, RM_HUED, RM_VALD, RM_PREV, _______,
                                _______, _______, _______, _______,_______, _______, _______, CG_TOGG, _______, _______,
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
    [L_GALLIUM] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU),  ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(CW_WIN, CCW_WIN), ENCODER_CCW_CW(APP_CW, APP_CCW)  },
    [L_SYM] = {     ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______)  },
    [L_NAV] = {     ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(CW_WIN, CCW_WIN)  },
    [L_NUM] = {     ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(CW_WIN, CCW_WIN)  },
    [L_MS] = {      ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(MS_WHLU, MS_WHLD), ENCODER_CCW_CW(_______, _______)  },
    [L_ADJUST] = {  ENCODER_CCW_CW(_______, _______),  ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______), ENCODER_CCW_CW(_______, _______)  }
};
#endif


bool remember_last_key_user(uint16_t keycode, keyrecord_t* record,
                            uint8_t* remembered_mods) {
    switch (keycode) {
        case CW_TOGG:
        case KC_ESC:
        case LARCANE:
        case RARCANE:
        case SENTENCE_CASE_TOGGLE:
        case APP_CW:
        case APP_CCW:
        case APP_KNB:
            return false;  // Magic keys will ignore the above keycodes.
    }
    return true;  // Other keys can be repeated.
}


void double_tap(uint16_t keycode) {
    tap_code16(keycode);
    wait_ms(50);
    tap_code16(keycode);
}

// Combo key handling
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Handle key press events
    if (record->event.pressed) {
        switch (keycode) {
            // Arcane keys
            case LARCANE:
                process_left_arcane(get_last_keycode(), get_last_mods());
                return false;
            case RARCANE:
                process_right_arcane(get_last_keycode(), get_last_mods());
                return false;

            // App switcher controls
            case APP_CW:
                app_switch(KC_LEFT);
                return false;
            case APP_CCW:
                app_switch(KC_RIGHT);
                return false;
            case APP_KNB:
                end_app_switcher_with_selection();
                return false;

            // Mouse double click
            case MS_DBL:
                double_tap(MS_BTN1);
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
        case SFTLLCK:
            if (record->tap.count && record->event.pressed) {
                // Toggle the lock on the highest layer.
                layer_lock_invert(get_highest_layer(layer_state));
                return false;
            }
            break;
        case MOUSE:
            if (record->tap.count && record->event.pressed) {
                tap_code16(PREFIX);  // Send Ctrl+Space on tap
                return false;
            }
            return true;  // Let QMK handle hold (layer activation)
    }

    return true;
}

void toggle_xcase(uint16_t keycode) {
    if (is_xcase_active() ){
        disable_xcase();
    } else {
        enable_xcase_with(keycode);
    }
}

void leader_start_user(void){
    is_leader_active = true;
    is_leader_changed = true;
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
    } else if (leader_sequence_two_keys(SE_E, SE_P)){
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
    }


    is_leader_active = false;
    is_leader_changed = true;
    leader_active_synced = 0;
}

void sentence_case_sync_handler(uint8_t in_size, const void* in_data,
                                uint8_t out_size, void* out_data) {
    if (in_size == sizeof(sentence_case_on_synced)) {
        memcpy(&sentence_case_on_synced, in_data, sizeof(sentence_case_on_synced));
    }
}

void sentence_case_primed_sync_handler(uint8_t in_size, const void* in_data,
                                       uint8_t out_size, void* out_data) {
    if (in_size == sizeof(sentence_case_primed_synced)) {
        memcpy(&sentence_case_primed_synced, in_data, sizeof(sentence_case_primed_synced));
    }
}

void leader_sync_handler(uint8_t in_size, const void* in_data,
                         uint8_t out_size, void* out_data) {
    if (in_size == sizeof(leader_active_synced)) {
        memcpy(&leader_active_synced, in_data, sizeof(leader_active_synced));
    }
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(SENTENCE_CASE_SYNC, sentence_case_sync_handler);
    transaction_register_rpc(SENTENCE_CASE_PRIMED_SYNC, sentence_case_primed_sync_handler);
    transaction_register_rpc(LEADER_SYNC, leader_sync_handler);
}

void housekeeping_task_user(void) {
    if (is_keyboard_master()) {
        static uint8_t last_sentence        = 0xFF;
        static uint8_t last_sentence_primed = 0xFF;
        static uint8_t last_leader          = 0xFF;
        uint8_t now_sentence                = is_sentence_case_on() ? 1 : 0;
        uint8_t now_sentence_primed         = is_sentence_case_primed() ? 1 : 0;
        uint8_t now_leader                  = is_leader_active ? 1 : 0;

        sentence_case_on_synced     = now_sentence;
        sentence_case_primed_synced = now_sentence_primed;
        leader_active_synced        = now_leader;

        if (now_sentence != last_sentence && is_transport_connected()) {
            transaction_rpc_send(SENTENCE_CASE_SYNC, sizeof(now_sentence), &now_sentence);
            last_sentence = now_sentence;
        }

        if (now_sentence_primed != last_sentence_primed && is_transport_connected()) {
            transaction_rpc_send(SENTENCE_CASE_PRIMED_SYNC, sizeof(now_sentence_primed), &now_sentence_primed);
            last_sentence_primed = now_sentence_primed;
        }

        if (now_leader != last_leader && is_transport_connected()) {
            transaction_rpc_send(LEADER_SYNC, sizeof(now_leader), &now_leader);
            last_leader = now_leader;
        }
    }
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

    if (leader_active_synced) {
        set_layer_rgb_by_configs(&leader_layer_config);
    }

    // Apply RGB for the highest active layer
    uint8_t layer = get_highest_layer(layer_state);
    switch (layer) {
        case L_NUM:
            set_layer_rgb_by_configs(&num_layer_config);
            break;
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
    }

    if ((get_oneshot_mods() | get_oneshot_locked_mods()) & MOD_MASK_SHIFT) {
        set_layer_rgb_by_configs(&oneshot_shift_layer_config);
    }

    if (sentence_case_primed_synced) {
        set_layer_rgb_by_configs(&sentence_case_primed_layer_config);
    } else if (sentence_case_on_synced) {
        set_layer_rgb_by_configs(&sentence_case_layer_config);
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


// Sentence Case behavior for Swedish layout
// Adds support for äåö as letters and ?! as sentence-ending punctuation
char sentence_case_press_user(uint16_t keycode, keyrecord_t* record, uint8_t mods) {
    const uint16_t original_keycode = keycode;
    // Unwrap "modded keycodes" like S(KC_MINS) used by SE_QUES.
    if (IS_QK_MODS(keycode)) {
        mods |= QK_MODS_GET_MODS(keycode);
        keycode = QK_MODS_GET_BASIC_KEYCODE(keycode);
    }

    // Only allow Shift and RAlt (AltGr) as active modifiers.
    if ((mods & ~(MOD_MASK_SHIFT | MOD_BIT(KC_RALT))) == 0) {
        const bool shifted = (mods & MOD_MASK_SHIFT) != 0;
        const bool is_closing_paren_key = (original_keycode == SE_RPRN) ||
                                          (keycode == KC_9 && shifted);
        const bool is_double_quote_key = (original_keycode == SE_DQUO) ||
                                         (keycode == KC_2 && shifted);

        if (is_closing_paren_key || is_double_quote_key) {
            return '\'';
        }

        switch (keycode) {
            case LARCANE:
            case RARCANE:
                return '\'';

            // Letters: A-Z plus Swedish ÅÄÖ
            case KC_A ... KC_Z:
            case KC_LBRC:  // SE_ARNG (Å)
            case KC_QUOT:  // SE_ADIA (Ä)
            case KC_SCLN:  // SE_ODIA (Ö)
                return 'a';

            // Sentence-ending punctuation: . ? !
            case KC_DOT:   // . is punctuation, Shift+. is symbol (>)
                return !shifted ? '.' : '#';
            case KC_1:     // Shift+1 = ! (SE_EXLM)
                return shifted ? '.' : '#';
            case KC_MINS:  // Shift+- = ? (SE_QUES) on Swedish layout
                return shifted ? '.' : '#';

            // Space
            case KC_SPC:
                return ' ';

            case KC_NUHS:  // SE_QUOT (')
                return '\'';

            // Symbol keys (backspaceable but not part of a word)
            case KC_2 ... KC_0:
            case KC_AT ... KC_RPRN:
            case KC_UNDS ... KC_COLN:
            case KC_GRV:
            case KC_COMM:
            case KC_SLSH:
                return '#';
        }
    }

    // For any other key, clear state and ignore.
    sentence_case_clear();
    return '\0';
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
        case SPC_NUM:
            if (other_keycode == KC_D || other_keycode == GH_LHM3 ||
                other_keycode == GH_LHM2 || other_keycode == GH_LHM4 ||
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
