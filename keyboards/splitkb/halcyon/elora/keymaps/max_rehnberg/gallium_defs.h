#ifndef GALLIUM_DEFS_H
#define GALLIUM_DEFS_H

// Home row positions
#define HM1 MOD_LGUI
#define HM2 MOD_LALT
#define HM3 MOD_LSFT
#define HM4 MOD_LCTL

// Home row modifier keycodes (for non-modtap usage)
#define KC_HM1 KC_LGUI
#define KC_HM2 KC_LALT
#define KC_HM3 KC_LSFT
#define KC_HM4 KC_LCTL

// Gallium
// Left side
#define GH_LHM4 MT(HM4, SE_C)
#define GH_LHM3 MT(HM3, SE_T)
#define GH_LHM2 MT(HM2, SE_R)
#define GH_LHM1 MT(HM1, SE_N)
// Right side
#define GH_RHM4 MT(HM4, SE_H)
#define GH_RHM3 MT(HM3, SE_A)
#define GH_RHM2 MT(HM2, SE_E)
#define GH_RHM1 MT(HM1, SE_I)

// Nav
// Right side
#define NV_RHM4 MT(HM4, KC_4)
#define NV_RHM3 MT(HM3, KC_5)
#define NV_RHM2 MT(HM2, KC_6)
#define NV_RHM1 MT(HM1, BKSLSH)

#endif // GALLIUM_DEFS_H
