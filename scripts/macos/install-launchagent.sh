#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
TEMPLATE="$SCRIPT_DIR/com.max.qmk-nano-hid.plist.template"
TARGET_DIR="$HOME/Library/LaunchAgents"
TARGET_PLIST="$TARGET_DIR/com.max.qmk-nano-hid.plist"

if [[ ! -f "$TEMPLATE" ]]; then
  echo "Missing template: $TEMPLATE" >&2
  exit 1
fi

PYTHON_PATH="${PYTHON_PATH:-$(command -v python3)}"
if [[ -z "$PYTHON_PATH" ]]; then
  echo "Could not find python3 in PATH" >&2
  exit 1
fi

mkdir -p "$TARGET_DIR"

sed \
  -e "s#{{PYTHON_PATH}}#$PYTHON_PATH#g" \
  -e "s#{{REPO_PATH}}#$REPO_ROOT#g" \
  -e "s#{{HOME}}#$HOME#g" \
  "$TEMPLATE" > "$TARGET_PLIST"

chmod 644 "$TARGET_PLIST"

launchctl bootout "gui/$(id -u)" "$TARGET_PLIST" >/dev/null 2>&1 || true
launchctl bootstrap "gui/$(id -u)" "$TARGET_PLIST"

echo "Installed LaunchAgent: $TARGET_PLIST"
echo "Python path: $PYTHON_PATH"
echo "You may need to grant Input Monitoring permission to this python binary."
