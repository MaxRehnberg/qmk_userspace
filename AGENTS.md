# Repository guidance

This is a personal QMK userspace. Prefer small changes that follow the existing keymap structure.

## Primary profile

The main Elora profile is:

`keyboards/splitkb/halcyon/elora/keymaps/max_rehnberg/`

Start with:

- `keymap.c` — layers and keyboard behavior
- `config.h` — timing and feature configuration
- `rules.mk` — enabled features and source files
- `keymap.json` — community modules

## Building

Use the root `Makefile`:

```sh
make clean && make elora
make elora-left
make elora-right
make ploopy
```

Do not use `qmk userspace-compile` for the Elora halves. Its global environment can mix Elora and Ploopy build artifacts.

The QMK checkout is configured through `qmk config user.qmk_home`.

Update the QMK checkout and its submodules only when explicitly requested:

```sh
make update-qmk
```

Do not run this casually or as part of a normal build. It changes the QMK and dependency revisions used to compile the firmware. After an intentional update, run a clean build.

## Ploopy integration

The integration has three parts:

- Ploopy Nano firmware: `keyboards/ploopyco/nano_2/rev2_003/keymaps/max_rehnberg/`
- Elora-side receiver and mouse-layer handling: `keyboards/splitkb/halcyon/elora/keymaps/max_rehnberg/ploopy.c`
- Host-side HID adapter/bridge: `scripts/README.md` and `scripts/nano_hid.py`

Read the relevant README before changing the bridge protocol. Keep packet definitions and device identifiers consistent across the Nano firmware, host adapter, and Elora receiver.

## Dependencies

Some dependencies are Git submodules. Avoid modifying submodule contents unless explicitly requested. Prefer implementing keymap-specific behavior within this userspace.

## Verification

After C or QMK configuration changes:

```sh
git diff --check
make clean && make elora
```

Build Ploopy separately when its firmware or shared HID behavior changes:

```sh
make ploopy
```
