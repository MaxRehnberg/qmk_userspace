# Ploopy Nano host integration

Makes the standalone Ploopy Nano temporarily activate the Mouse layer on the
Elora, Kyria, or both.

The Nano is a separate USB trackball, so the keyboard cannot observe its
movement directly. On macOS, `nano_hid.py bridge` reads the Nano's QMK Raw HID
`move-start` events and forwards the same versioned 32-byte `PN` packet to each
explicitly selected keyboard Raw HID endpoint.

Windows and Linux can continue using the Nano firmware's existing Scroll Lock
beacon without the bridge. The host forwards those lock-state changes as
ordinary HID indicator reports, and both keyboard firmwares recognize the same
two-transition beacon.

## macOS data flow

```text
Ploopy Nano Raw HID
    -> nano_hid.py bridge
    -> configured Raw HID targets
       -> Elora
       -> Kyria dongle
```

All Raw HID endpoints use:

- usage page: `0xFF60`
- usage: `0x61`
- packet: 32 bytes with `PN` magic, protocol version `1`, and `MOVE_START` event

Device identities differ:

- Nano/Ploopy vendor id: `0x5043`
- Elora vendor id: `0x8D1D`
- Kyria dongle vendor/product id: `0x1D50` / `0x615E`

## Target selection

Routing is explicit in `bridge.toml`:

```toml
[bridge]
targets = ["kyria"]
```

Use `targets = ["elora"]` for Elora, or
`targets = ["elora", "kyria"]` to forward to both. Merely connecting a keyboard
does not select it. Each configured target reconnects independently, so one
missing keyboard does not prevent delivery to another.

If multiple devices match one target, set that target's exact HID `path` rather
than allowing an arbitrary choice.

An older configuration containing only `[elora]` remains Elora-only when
`bridge.targets` is absent. New configurations default to Kyria, but specifying
the targets is recommended.

## Fresh macOS setup

From the QMK userspace root:

```bash
brew install uv hidapi
uv venv .venv
uv pip install --python .venv/bin/python -r scripts/requirements.txt

mkdir -p ~/.config/qmk-nano-hid
cp scripts/bridge.example.toml ~/.config/qmk-nano-hid/bridge.toml
```

Plug in the Nano and intended keyboard, then verify Nano discovery:

```bash
export DYLD_FALLBACK_LIBRARY_PATH="/opt/homebrew/opt/hidapi/lib:/usr/local/opt/hidapi/lib"
.venv/bin/python scripts/nano_hid.py list
```

Test the configured route manually:

```bash
.venv/bin/python scripts/nano_hid.py bridge --verbose
```

The bridge logs each connected target and every successful destination in
verbose mode. If moving the Nano activates the intended Mouse layer, install
auto-start:

```bash
PYTHON_PATH="$PWD/.venv/bin/python" scripts/macos/install-launchagent.sh
```

The agent installs to:

```text
~/Library/LaunchAgents/com.max.qmk-nano-hid.plist
```

Logs:

```text
~/Library/Logs/qmk-nano-hid.out.log
~/Library/Logs/qmk-nano-hid.err.log
```

## Useful commands

Send Nano commands:

```bash
.venv/bin/python scripts/nano_hid.py send toggle-scroll
.venv/bin/python scripts/nano_hid.py send cycle-dpi
.venv/bin/python scripts/nano_hid.py send bootloader
```

Listen for Nano events:

```bash
.venv/bin/python scripts/nano_hid.py listen --json --wait-for-device
```

Check or restart the LaunchAgent:

```bash
launchctl print "gui/$(id -u)/com.max.qmk-nano-hid"
launchctl bootout "gui/$(id -u)" ~/Library/LaunchAgents/com.max.qmk-nano-hid.plist
launchctl bootstrap "gui/$(id -u)" ~/Library/LaunchAgents/com.max.qmk-nano-hid.plist
```

## Troubleshooting

- `No matching RAW HID device found`: confirm the relevant device firmware
  exposes Raw HID and inspect paths with `nano_hid.py list`.
- Multiple matching devices: copy the intended HID `path` into its section in
  `~/.config/qmk-nano-hid/bridge.toml`.
- Python cannot load HID: ensure `brew install hidapi` is done and
  `DYLD_FALLBACK_LIBRARY_PATH` is exported as shown above. The LaunchAgent sets
  this automatically.
- macOS blocks HID access: grant Input Monitoring permission to
  `<repo>/.venv/bin/python`.
- A target remains unavailable: run `bridge --verbose`, verify its VID/PID and
  Raw HID usage, and reflash that keyboard if its endpoint is missing.
- Events are forwarded but Mouse does not activate: verify the target name in
  `bridge.targets` and check that it appears in the verbose `targets=` output.
