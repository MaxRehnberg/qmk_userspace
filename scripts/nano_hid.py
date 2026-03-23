#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
import os
import sys
import time
from pathlib import Path
from typing import Any

try:
    import tomllib
except ModuleNotFoundError:  # pragma: no cover
    import tomli as tomllib  # type: ignore

try:
    import hid  # type: ignore[import-not-found]
except ImportError as exc:  # pragma: no cover
    print("Missing dependency: hid", file=sys.stderr)
    print("Install with: python3 -m pip install hid", file=sys.stderr)
    raise SystemExit(1) from exc


NANO_MAGIC_0 = ord("P")
NANO_MAGIC_1 = ord("N")
NANO_VERSION = 1
NANO_TYPE_COMMAND = 0x01
NANO_TYPE_EVENT = 0x02
NANO_PACKET_SIZE = 32

DEFAULT_USAGE_PAGE = 0xFF60
DEFAULT_USAGE = 0x61
DEFAULT_CONFIG_PATH = os.path.expanduser("~/.config/qmk-nano-hid/config.toml")
DEFAULT_BRIDGE_CONFIG_PATH = os.path.expanduser("~/.config/qmk-nano-hid/bridge.toml")

COMMAND_IDS = {
    "toggle-scroll": 0x01,
    "bootloader": 0x02,
    "cycle-dpi": 0x03,
}

EVENT_NAMES = {
    0x81: "move-start",
    0x82: "move-stop",
    0x83: "scroll-state",
}

BRIDGE_DEFAULT_PLOOPY_FILTER = {
    "usage_page": DEFAULT_USAGE_PAGE,
    "usage": DEFAULT_USAGE,
    "vendor_id": 0x5043,
    "product_id": None,
    "path": None,
}

BRIDGE_DEFAULT_ELORA_FILTER = {
    "usage_page": DEFAULT_USAGE_PAGE,
    "usage": DEFAULT_USAGE,
    "vendor_id": 0x8D1D,
    "product_id": None,
    "path": None,
}


def to_int(value: Any) -> int | None:
    if value is None:
        return None
    if isinstance(value, int):
        return value
    if isinstance(value, str):
        return int(value, 0)
    raise ValueError(f"Unsupported integer value: {value!r}")


def hid_error_types() -> tuple[type[Exception], ...]:
    hid_exc = getattr(hid, "HIDException", None)
    if isinstance(hid_exc, type) and issubclass(hid_exc, Exception):
        return (OSError, hid_exc)
    return (OSError,)


def merge_device_filter(
    defaults: dict[str, Any], overrides: dict[str, Any] | None
) -> dict[str, Any]:
    merged = dict(defaults)
    if isinstance(overrides, dict):
        merged.update(overrides)

    return {
        "usage_page": int(to_int(merged.get("usage_page")) or DEFAULT_USAGE_PAGE),
        "usage": int(to_int(merged.get("usage")) or DEFAULT_USAGE),
        "vendor_id": to_int(merged.get("vendor_id")),
        "product_id": to_int(merged.get("product_id")),
        "path": merged.get("path"),
    }


def matches_device(device: dict[str, Any], device_filter: dict[str, Any]) -> bool:
    if int(device.get("usage_page", -1)) != int(device_filter["usage_page"]):
        return False
    if int(device.get("usage", -1)) != int(device_filter["usage"]):
        return False

    vendor_id = device_filter.get("vendor_id")
    if vendor_id is not None and int(device.get("vendor_id", -1)) != int(vendor_id):
        return False

    product_id = device_filter.get("product_id")
    if product_id is not None and int(device.get("product_id", -1)) != int(product_id):
        return False

    selected_path = device_filter.get("path")
    if selected_path is not None and decode_path(device.get("path")) != str(
        selected_path
    ):
        return False

    return True


def decode_device_identity(device: dict[str, Any]) -> str:
    return (
        f"vid=0x{int(device.get('vendor_id', 0)):04X} "
        f"pid=0x{int(device.get('product_id', 0)):04X} "
        f"path={decode_path(device.get('path'))}"
    )


def best_device_match(
    devices: list[dict[str, Any]], device_filter: dict[str, Any]
) -> dict[str, Any] | None:
    matches = [dev for dev in devices if matches_device(dev, device_filter)]
    if not matches:
        return None

    chosen = sorted(matches, key=lambda dev: decode_path(dev.get("path")))[0]
    return chosen


def sleep_remaining(deadline: float | None, retry_seconds: float) -> None:
    if deadline is None:
        time.sleep(retry_seconds)
        return

    remaining = deadline - time.monotonic()
    if remaining <= 0:
        return

    time.sleep(min(retry_seconds, remaining))


def open_with_retry(
    device_filter: dict[str, Any],
    retry_seconds: float,
    deadline: float | None,
    label: str,
) -> tuple[dict[str, Any], Any]:
    io_errors = hid_error_types()
    retry_errors = (RuntimeError,) + io_errors

    while True:
        if deadline is not None and time.monotonic() >= deadline:
            raise RuntimeError(f"Timed out waiting for {label} device")

        try:
            dev_info = select_single_device(device_filter)
            dev = open_device(dev_info)
            return dev_info, dev
        except retry_errors as exc:
            print(f"{label}: {exc}; retrying in {retry_seconds:.1f}s", file=sys.stderr)
            sleep_remaining(deadline, retry_seconds)


def read_next_parsed_event(
    dev: Any, timeout_ms: int, verbose: bool
) -> dict[str, int] | None:
    packet = read_packet(dev, timeout_ms)
    if packet is None:
        return None

    parsed = parse_packet(packet)
    if parsed is None:
        if verbose:
            print(f"ignored non-nano packet: {packet.hex()}")
        return None

    if parsed["type"] != NANO_TYPE_EVENT:
        if verbose:
            print(f"ignored packet type=0x{parsed['type']:02X}")
        return None

    return parsed


def make_elora_move_packet(sequence: int = 0) -> bytes:
    return build_packet(NANO_TYPE_EVENT, 0x81, 0, sequence)


def cmd_bridge(args: argparse.Namespace) -> int:
    bridge_config = load_config(args.bridge_config)
    if not isinstance(bridge_config, dict):
        bridge_config = {}

    source_filter = merge_device_filter(
        BRIDGE_DEFAULT_PLOOPY_FILTER,
        bridge_config.get("ploopy") if isinstance(bridge_config, dict) else None,
    )
    sink_filter = merge_device_filter(
        BRIDGE_DEFAULT_ELORA_FILTER,
        bridge_config.get("elora") if isinstance(bridge_config, dict) else None,
    )

    retry_seconds = max(args.retry_seconds, 0.2)
    read_timeout_ms = max(args.timeout_ms, 20)
    quiet_timeout = args.quiet_timeout
    io_errors = hid_error_types()
    bridge_errors = (RuntimeError,) + io_errors

    if quiet_timeout is not None and quiet_timeout <= 0:
        quiet_timeout = None

    deadline = None
    if args.seconds is not None:
        deadline = time.monotonic() + args.seconds

    print(
        "Bridge started: "
        f"source usage=0x{source_filter['usage']:02X} vid={source_filter['vendor_id']} -> "
        f"sink usage=0x{sink_filter['usage']:02X} vid={sink_filter['vendor_id']}",
        file=sys.stderr,
    )

    src_info: dict[str, Any] | None = None
    src_dev: Any | None = None
    dst_info: dict[str, Any] | None = None
    dst_dev: Any | None = None

    last_forwarded_at = 0.0
    events_seen = 0
    events_forwarded = 0

    def close_dev(dev: Any | None) -> None:
        if dev is None:
            return
        try:
            dev.close()
        except OSError:
            pass

    while True:
        if deadline is not None and time.monotonic() >= deadline:
            break

        try:
            if src_dev is None:
                src_info, src_dev = open_with_retry(
                    source_filter, retry_seconds, deadline, "source"
                )
                print(
                    f"source connected: {decode_device_identity(src_info)}",
                    file=sys.stderr,
                )

            if dst_dev is None:
                dst_info, dst_dev = open_with_retry(
                    sink_filter, retry_seconds, deadline, "sink"
                )
                print(
                    f"sink connected: {decode_device_identity(dst_info)}",
                    file=sys.stderr,
                )

            parsed = read_next_parsed_event(src_dev, read_timeout_ms, args.verbose)
            if parsed is None:
                continue

            events_seen += 1
            event_id = parsed["id"]
            now = time.monotonic()

            should_forward = event_id == 0x81
            if should_forward and quiet_timeout is not None:
                if (now - last_forwarded_at) < quiet_timeout:
                    should_forward = False

            if not should_forward:
                continue

            packet = make_elora_move_packet(parsed["seq"])
            try:
                write_packet(dst_dev, packet)
                last_forwarded_at = now
                events_forwarded += 1

                if args.verbose:
                    print(
                        "forwarded move-start "
                        f"src_seq={parsed['seq']} count={events_forwarded}",
                        file=sys.stderr,
                    )
            except bridge_errors as exc:
                print(f"sink write failed: {exc}; reconnecting sink", file=sys.stderr)
                close_dev(dst_dev)
                dst_dev = None

        except bridge_errors as exc:
            print(f"bridge I/O error: {exc}; reconnecting", file=sys.stderr)
            close_dev(src_dev)
            close_dev(dst_dev)
            src_dev = None
            dst_dev = None
            sleep_remaining(deadline, retry_seconds)

    close_dev(src_dev)
    close_dev(dst_dev)

    print(
        f"bridge stopped: seen={events_seen} forwarded={events_forwarded}",
        file=sys.stderr,
    )
    return 0


def parse_int(value: str) -> int:
    return int(value, 0)


def decode_path(raw_path: object) -> str:
    if isinstance(raw_path, bytes):
        return raw_path.decode("utf-8", errors="replace")
    if raw_path is None:
        return ""
    return str(raw_path)


def normalize_packet(raw_data: bytes) -> bytes | None:
    data = raw_data

    if len(data) == NANO_PACKET_SIZE + 1 and data[0] == 0:
        data = data[1:]
    elif len(data) > NANO_PACKET_SIZE:
        data = data[:NANO_PACKET_SIZE]

    if len(data) != NANO_PACKET_SIZE:
        return None

    return data


def build_packet(packet_type: int, packet_id: int, arg: int, sequence: int) -> bytes:
    packet = bytearray(NANO_PACKET_SIZE)
    packet[0] = NANO_MAGIC_0
    packet[1] = NANO_MAGIC_1
    packet[2] = NANO_VERSION
    packet[3] = packet_type & 0xFF
    packet[4] = packet_id & 0xFF
    packet[5] = arg & 0xFF
    packet[6] = (arg >> 8) & 0xFF
    packet[7] = sequence & 0xFF
    return bytes(packet)


def parse_packet(packet: bytes) -> dict[str, int] | None:
    if len(packet) != NANO_PACKET_SIZE:
        return None
    if packet[0] != NANO_MAGIC_0 or packet[1] != NANO_MAGIC_1:
        return None
    if packet[2] != NANO_VERSION:
        return None

    arg = packet[5] | (packet[6] << 8)
    return {
        "type": packet[3],
        "id": packet[4],
        "arg": arg,
        "seq": packet[7],
    }


def load_config(path: str) -> dict:
    config_path = Path(path)
    if not config_path.exists():
        return {}

    with config_path.open("rb") as fp:
        return tomllib.load(fp)


def as_int_or_none(value: Any) -> int | None:
    if value is None:
        return None
    return int(value)


def get_effective_device_filter(args: argparse.Namespace) -> dict[str, Any]:
    config = load_config(args.config)
    device_config = config.get("device", {}) if isinstance(config, dict) else {}

    usage_page = args.usage_page
    if usage_page is None:
        usage_page = int(device_config.get("usage_page", DEFAULT_USAGE_PAGE))

    usage = args.usage
    if usage is None:
        usage = int(device_config.get("usage", DEFAULT_USAGE))

    vendor_id = args.vid
    if vendor_id is None:
        vendor_id = as_int_or_none(device_config.get("vendor_id"))

    product_id = args.pid
    if product_id is None:
        product_id = as_int_or_none(device_config.get("product_id"))

    path = args.path
    if path is None and device_config.get("path"):
        path = str(device_config.get("path"))

    return {
        "usage_page": usage_page,
        "usage": usage,
        "vendor_id": vendor_id,
        "product_id": product_id,
        "path": path,
    }


def list_devices(device_filter: dict[str, Any]) -> list[dict[str, Any]]:
    usage_page = int(device_filter["usage_page"])
    usage = int(device_filter["usage"])
    vendor_id = device_filter["vendor_id"]
    product_id = device_filter["product_id"]
    selected_path = device_filter["path"]

    results: list[dict[str, Any]] = []
    for dev in hid.enumerate():
        if int(dev.get("usage_page", -1)) != usage_page:
            continue
        if int(dev.get("usage", -1)) != usage:
            continue
        if vendor_id is not None and int(dev.get("vendor_id", -1)) != int(vendor_id):
            continue
        if product_id is not None and int(dev.get("product_id", -1)) != int(product_id):
            continue
        if selected_path is not None and decode_path(dev.get("path")) != str(
            selected_path
        ):
            continue
        results.append(dev)

    return results


def select_single_device(device_filter: dict[str, Any]) -> dict[str, Any]:
    devices = list_devices(device_filter)
    if not devices:
        raise RuntimeError("No matching RAW HID device found")
    if len(devices) > 1 and device_filter["path"] is None:
        message_lines = [
            "Multiple matching RAW HID devices found; rerun with --path:",
        ]
        for dev in devices:
            message_lines.append(f"  - {decode_path(dev.get('path'))}")
        raise RuntimeError("\n".join(message_lines))
    return devices[0]


def hid_has_legacy_api() -> bool:
    return hasattr(hid, "device")


def hid_has_class_api() -> bool:
    return hasattr(hid, "Device")


def open_device(device_info: dict[str, Any]) -> Any:
    path = device_info.get("path")

    if hid_has_legacy_api():
        dev = hid.device()

        if path:
            if isinstance(path, str):
                dev.open_path(path.encode("utf-8"))
            else:
                dev.open_path(path)
        else:
            dev.open(int(device_info["vendor_id"]), int(device_info["product_id"]))

        return dev

    if hid_has_class_api():
        if path:
            if isinstance(path, str):
                return hid.Device(path=path.encode("utf-8"))
            return hid.Device(path=path)

        return hid.Device(
            vid=int(device_info["vendor_id"]),
            pid=int(device_info["product_id"]),
        )

    raise RuntimeError("Unsupported hid module API")


def write_packet(dev: Any, packet: bytes) -> None:
    first = b"\x00" + packet
    fallback = packet

    for payload in (first, fallback):
        try:
            written = dev.write(payload)
        except OSError:
            continue
        if written > 0:
            return

    raise RuntimeError("Failed to write RAW HID packet")


def read_packet(dev: Any, timeout_ms: int) -> bytes | None:
    raw = dev.read(NANO_PACKET_SIZE + 1, timeout_ms)
    if not raw:
        return None

    packet = normalize_packet(bytes(raw))
    return packet


def format_event(parsed: dict[str, int]) -> dict[str, object]:
    event_id = parsed["id"]
    payload: dict[str, object] = {
        "event": EVENT_NAMES.get(event_id, f"unknown-0x{event_id:02X}"),
        "event_id": event_id,
        "arg": parsed["arg"],
        "seq": parsed["seq"],
    }

    if event_id == 0x83:
        payload["scroll_enabled"] = parsed["arg"] != 0

    return payload


def cmd_list(args: argparse.Namespace) -> int:
    device_filter = get_effective_device_filter(args)
    devices = list_devices(device_filter)

    if not devices:
        print("No matching RAW HID devices found")
        return 1

    for idx, dev in enumerate(devices, start=1):
        print(
            f"{idx}. vid=0x{int(dev.get('vendor_id', 0)):04X} "
            f"pid=0x{int(dev.get('product_id', 0)):04X} "
            f"usage_page=0x{int(dev.get('usage_page', 0)):04X} "
            f"usage=0x{int(dev.get('usage', 0)):02X}"
        )
        print(f"   product={dev.get('product_string', '')}")
        print(f"   path={decode_path(dev.get('path'))}")

    return 0


def cmd_send(args: argparse.Namespace) -> int:
    device_filter = get_effective_device_filter(args)
    command_id = COMMAND_IDS[args.command]
    packet = build_packet(NANO_TYPE_COMMAND, command_id, args.arg, args.seq)

    dev_info = select_single_device(device_filter)
    dev = open_device(dev_info)
    try:
        write_packet(dev, packet)
    finally:
        dev.close()

    print(
        f"Sent command {args.command} "
        f"(id=0x{command_id:02X}, arg={args.arg}, seq={args.seq})"
    )
    return 0


def cmd_listen(args: argparse.Namespace) -> int:
    device_filter = get_effective_device_filter(args)
    retry_seconds = max(args.retry_seconds, 0.2)
    io_errors = hid_error_types()
    listen_errors = (RuntimeError,) + io_errors
    deadline = None

    if args.seconds is not None:
        deadline = time.monotonic() + args.seconds

    while True:
        if deadline is not None and time.monotonic() >= deadline:
            return 0

        try:
            dev_info = select_single_device(device_filter)
            dev = open_device(dev_info)
        except listen_errors as exc:
            if not args.wait_for_device:
                raise RuntimeError(str(exc)) from exc

            print(f"{exc}; retrying in {retry_seconds:.1f}s", file=sys.stderr)
            time.sleep(retry_seconds)
            continue

        try:
            print(f"Listening on {decode_path(dev_info.get('path'))}")

            while True:
                if deadline is not None and time.monotonic() >= deadline:
                    return 0

                packet = read_packet(dev, args.timeout_ms)
                if packet is None:
                    continue

                parsed = parse_packet(packet)
                if parsed is None:
                    if args.verbose:
                        print(f"ignored non-nano packet: {packet.hex()}")
                    continue

                if parsed["type"] != NANO_TYPE_EVENT:
                    if args.verbose:
                        print(f"ignored packet type=0x{parsed['type']:02X}")
                    continue

                event = format_event(parsed)
                if args.json:
                    print(json.dumps(event, sort_keys=True))
                else:
                    print(
                        f"event={event['event']} id=0x{event['event_id']:02X} "
                        f"arg={event['arg']} seq={event['seq']}"
                    )
        except io_errors as exc:
            if not args.wait_for_device:
                raise RuntimeError(f"HID read failed: {exc}") from exc

            print(
                f"HID device unavailable; retrying in {retry_seconds:.1f}s",
                file=sys.stderr,
            )
            time.sleep(retry_seconds)
        finally:
            dev.close()

        if not args.wait_for_device:
            return 0

    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="QMK Ploopy Nano RAW HID helper")
    parser.add_argument(
        "--config", default=DEFAULT_CONFIG_PATH, help="config file path"
    )
    parser.add_argument("--vid", type=parse_int, help="vendor id (e.g. 0x5043)")
    parser.add_argument("--pid", type=parse_int, help="product id")
    parser.add_argument(
        "--usage-page", type=parse_int, help="usage page (default 0xFF60)"
    )
    parser.add_argument("--usage", type=parse_int, help="usage id (default 0x61)")
    parser.add_argument("--path", help="exact HID path (from list command)")
    parser.add_argument(
        "--bridge-config",
        default=DEFAULT_BRIDGE_CONFIG_PATH,
        help="bridge config file path",
    )

    subparsers = parser.add_subparsers(dest="subcommand", required=True)

    subparsers.add_parser("list", help="list matching RAW HID devices")

    send_parser = subparsers.add_parser("send", help="send a command packet")
    send_parser.add_argument("command", choices=sorted(COMMAND_IDS.keys()))
    send_parser.add_argument(
        "--arg", type=parse_int, default=0, help="uint16 command argument"
    )
    send_parser.add_argument(
        "--seq", type=parse_int, default=0, help="packet sequence byte"
    )

    listen_parser = subparsers.add_parser("listen", help="listen for event packets")
    listen_parser.add_argument(
        "--seconds", type=float, help="listen duration in seconds"
    )
    listen_parser.add_argument(
        "--timeout-ms", type=int, default=250, help="read timeout in ms"
    )
    listen_parser.add_argument(
        "--json", action="store_true", help="print events as JSON"
    )
    listen_parser.add_argument(
        "--verbose", action="store_true", help="print ignored packets"
    )
    listen_parser.add_argument(
        "--wait-for-device",
        action="store_true",
        help="keep retrying until a matching device appears",
    )
    listen_parser.add_argument(
        "--retry-seconds",
        type=float,
        default=2.0,
        help="retry delay when waiting for device",
    )

    bridge_parser = subparsers.add_parser(
        "bridge", help="forward Nano move events to Elora RAW HID"
    )
    bridge_parser.add_argument(
        "--seconds", type=float, help="bridge duration in seconds"
    )
    bridge_parser.add_argument(
        "--timeout-ms", type=int, default=80, help="source read timeout in ms"
    )
    bridge_parser.add_argument(
        "--quiet-timeout",
        type=float,
        default=0.08,
        help="minimum seconds between forwarded move packets",
    )
    bridge_parser.add_argument(
        "--retry-seconds",
        type=float,
        default=2.0,
        help="retry delay when reconnecting devices",
    )
    bridge_parser.add_argument(
        "--verbose", action="store_true", help="print bridge debug messages"
    )

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    try:
        if args.subcommand == "list":
            return cmd_list(args)
        if args.subcommand == "send":
            return cmd_send(args)
        if args.subcommand == "listen":
            return cmd_listen(args)
        if args.subcommand == "bridge":
            return cmd_bridge(args)
    except RuntimeError as exc:
        print(str(exc), file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        return 130

    parser.print_help()
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
