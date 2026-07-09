---
name: qmk-userspace-build
description: Build this QMK userspace safely using the repo Makefile
compatibility: opencode
metadata:
  audience: local-dev
  project: qmk_userspace
---

# QMK userspace build instructions

Use this skill when building firmware in this repo.

## Build commands

Run builds from the repository root:

```bash
make elora        # Build both Elora halves
make elora-left   # Build left half with HLC_ENCODER=1, TARGET=elora_left
make elora-right  # Build right half with HLC_NONE=1, TARGET=elora_right
make ploopy       # Build the Ploopy Nano target
```

The main verification command is:

```bash
make clean && make elora
```

## Important: avoid userspace-compile for Elora halves

Do **not** build Elora halves with commands like:

```bash
qmk userspace-compile -e HLC_ENCODER=1 -e TARGET=elora_left
```

`qmk userspace-compile` applies `-e TARGET=...` globally to every target in
`qmk.json`, including Ploopy. This can poison `.build/obj_elora_left` or
`.build/obj_elora_right` with generated files from the wrong keyboard and cause
misleading errors, for example:

- `RGB_MATRIX_LED_COUNT undeclared`
- `fatal error: ploopyco.h: No such file or directory`

If either happens, clean and rebuild with Make:

```bash
make clean && make elora
```

## Why the Makefile is preferred

The root `Makefile` builds each target directly with `qmk compile`, passing the
correct per-half environment only to that one target:

- Left: `HLC_ENCODER=1 TARGET=elora_left`
- Right: `HLC_NONE=1 TARGET=elora_right`

This avoids cross-target build artifact contamination.

## Generated artifacts

Successful Elora builds produce UF2 files in the userspace root:

- `elora_left.uf2`
- `elora_right.uf2`

They are also copied into the configured `qmk_firmware` folder by QMK.
