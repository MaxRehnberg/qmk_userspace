# Nano 2 keymap structure

- `keymap.c`: entry points and tap-dance behavior only
- `nano_core.c` / `nano_core.h`: shared command handling + movement event logic
- `nano_transport_shared.c`: OS detection and transport selection
- `nano_transport_windows_led.c`: LED beacon + command decoding (Scroll Lock)
- `nano_transport_rawhid.c`: Raw HID packet receive/send

This split keeps host-agnostic logic separate from host-specific transport details.
