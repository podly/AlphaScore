#!/usr/bin/env python3
"""Enable or disable AlphaScore Demo Mode over UART."""

from __future__ import annotations

import argparse
import sys
import time


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Enable or disable AlphaScore Demo Mode over UART."
    )
    parser.add_argument(
        "port",
        help="serial port, for example COM3 or /dev/ttyUSB0",
    )
    parser.add_argument(
        "state",
        choices=("on", "off"),
        help="desired Demo Mode state",
    )
    parser.add_argument(
        "--baud",
        type=int,
        default=115200,
        help="UART baud rate (default: 115200)",
    )
    parser.add_argument(
        "--startup-delay",
        type=float,
        default=2.0,
        help="delay after opening the port, in seconds (default: 2.0)",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.baud <= 0:
        print("error: baud rate must be positive", file=sys.stderr)
        return 2
    if args.startup_delay < 0:
        print("error: startup delay cannot be negative", file=sys.stderr)
        return 2

    try:
        import serial
    except ImportError:
        print(
            "error: pyserial is required; install it with: "
            f"{sys.executable} -m pip install pyserial",
            file=sys.stderr,
        )
        return 2

    command = "@X:1\n" if args.state == "on" else "@X:0\n"

    try:
        with serial.Serial(args.port, args.baud, timeout=1) as serial_port:
            if args.startup_delay:
                time.sleep(args.startup_delay)
            serial_port.write(command.encode("ascii"))
            serial_port.flush()
    except serial.SerialException as exc:
        print(f"error: cannot use {args.port}: {exc}", file=sys.stderr)
        return 1

    print(f"Demo Mode {args.state}: command sent to {args.port}.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
