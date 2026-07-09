# Ploopy Nano host integration

Makes the standalone Ploopy Nano wake the Elora mouse layer on macOS.

The Nano is a separate trackball, so the Elora does not automatically know when
it moves. `nano_hid.py bridge` listens for Nano movement over QMK Raw HID and
forwards a small activity packet to the Elora. The Elora then enables its mouse
layer briefly.

## How it works

- Nano emits Raw HID `move-start` events.
- The bridge script listens to the Nano.
- The bridge forwards activity packets to the Elora.
- A macOS LaunchAgent keeps the bridge running in the background.

Devices are discovered through QMK Raw HID:

- usage page: `0xFF60`
- usage: `0x61`
- Nano/Ploopy vendor id: `0x5043`
- Elora vendor id: `0x8D1D`

## Fresh macOS setup

From repo root:

```bash
brew install uv hidapi
uv venv .venv
uv pip install --python .venv/bin/python -r scripts/requirements.txt

mkdir -p ~/.config/qmk-nano-hid
cp scripts/bridge.example.toml ~/.config/qmk-nano-hid/bridge.toml
```

Plug in the Nano and Elora, then verify discovery:

```bash
export DYLD_FALLBACK_LIBRARY_PATH="/opt/homebrew/opt/hidapi/lib:/usr/local/opt/hidapi/lib"
.venv/bin/python scripts/nano_hid.py list
```

Test manually:

```bash
.venv/bin/python scripts/nano_hid.py bridge --verbose
```

If moving the Nano wakes the Elora mouse layer, install auto-start:

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

Check LaunchAgent status:

```bash
launchctl print "gui/$(id -u)/com.max.qmk-nano-hid"
```

Restart LaunchAgent:

```bash
launchctl bootout "gui/$(id -u)" ~/Library/LaunchAgents/com.max.qmk-nano-hid.plist
launchctl bootstrap "gui/$(id -u)" ~/Library/LaunchAgents/com.max.qmk-nano-hid.plist
```

## Troubleshooting

- `No matching RAW HID device found`: plug in the Nano/Elora, make sure firmware
  has `RAW_ENABLE = yes`, and run `nano_hid.py list`.
- Multiple matching devices: copy the correct HID `path` from `list` into
  `~/.config/qmk-nano-hid/bridge.toml`.
- Python cannot load HID: ensure `brew install hidapi` is done and
  `DYLD_FALLBACK_LIBRARY_PATH` is exported as shown above. The LaunchAgent sets
  this automatically.
- macOS blocks HID access: grant Input Monitoring permission to
  `<repo>/.venv/bin/python`.
- Bridge runs but the Elora layer does not wake: run `bridge --verbose`, confirm
  events are forwarded, and pin the Elora `path` if multiple QMK devices are
  connected.
