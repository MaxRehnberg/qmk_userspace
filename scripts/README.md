# Ploopy Nano Host Integration

This directory is organized by host-specific behavior:

- `nano_hid.py`: cross-platform helper used for the macOS Raw HID path
- `macos/`: LaunchAgent template and installer
- `linux/`: notes for native LED path (no script required)
- `bridge.example.toml`: optional config for Nano->Elora bridge mode

Windows is intentionally script-free; the Nano uses Scroll Lock LED beacon/command decoding there.

## Recommended setup (uv + venv)

From repo root:

```bash
uv venv .venv
uv pip install --python .venv/bin/python -r scripts/requirements.txt
```

Install hidapi if needed:

```bash
brew install hidapi
```

If terminal runs fail to load hidapi on macOS, run commands with:

```bash
DYLD_FALLBACK_LIBRARY_PATH="/opt/homebrew/opt/hidapi/lib:/usr/local/opt/hidapi/lib"
```

## Optional local config

Create a per-machine config (not committed):

```bash
mkdir -p ~/.config/qmk-nano-hid
cp scripts/config.example.toml ~/.config/qmk-nano-hid/config.toml
```

## Manual usage

List matching RAW HID devices:

```bash
DYLD_FALLBACK_LIBRARY_PATH="/opt/homebrew/opt/hidapi/lib:/usr/local/opt/hidapi/lib" \
  .venv/bin/python scripts/nano_hid.py list
```

Send commands:

```bash
DYLD_FALLBACK_LIBRARY_PATH="/opt/homebrew/opt/hidapi/lib:/usr/local/opt/hidapi/lib" \
  .venv/bin/python scripts/nano_hid.py send toggle-scroll

DYLD_FALLBACK_LIBRARY_PATH="/opt/homebrew/opt/hidapi/lib:/usr/local/opt/hidapi/lib" \
  .venv/bin/python scripts/nano_hid.py send cycle-dpi

DYLD_FALLBACK_LIBRARY_PATH="/opt/homebrew/opt/hidapi/lib:/usr/local/opt/hidapi/lib" \
  .venv/bin/python scripts/nano_hid.py send bootloader
```

Listen for Nano events:

```bash
DYLD_FALLBACK_LIBRARY_PATH="/opt/homebrew/opt/hidapi/lib:/usr/local/opt/hidapi/lib" \
  .venv/bin/python scripts/nano_hid.py listen --json --wait-for-device
```

Bridge Nano movement to Elora Raw HID (for auto mouse layer):

```bash
mkdir -p ~/.config/qmk-nano-hid
cp scripts/bridge.example.toml ~/.config/qmk-nano-hid/bridge.toml

DYLD_FALLBACK_LIBRARY_PATH="/opt/homebrew/opt/hidapi/lib:/usr/local/opt/hidapi/lib" \
  .venv/bin/python scripts/nano_hid.py bridge
```

Notes:

- Bridge forwards Nano `move-start` events to Elora as Raw HID activity packets.
- Source matching defaults to `vendor_id = 0x5043` (Ploopy).
- If more than one matching device exists, set `path` in `bridge.toml`.
- `--quiet-timeout` controls minimum send cadence (default `0.08s`).

## macOS auto-start

Install/update the LaunchAgent with the venv interpreter:

```bash
PYTHON_PATH="$PWD/.venv/bin/python" scripts/macos/install-launchagent.sh
```

The agent runs `bridge`, so it stays alive, reconnects when devices are unplugged/replugged, and forwards Nano movement activity to Elora.

The installer writes:

- `~/Library/LaunchAgents/com.max.qmk-nano-hid.plist`

Logs:

- `~/Library/Logs/qmk-nano-hid.out.log`
- `~/Library/Logs/qmk-nano-hid.err.log`

Status check:

```bash
launchctl print "gui/$(id -u)/com.max.qmk-nano-hid"
```

If macOS blocks HID access, grant Input Monitoring permission to the Python binary used by the agent (`.venv/bin/python`).
