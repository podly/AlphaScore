#!/usr/bin/env python3
"""Visual test of the 15-segment display serial input protocol."""

from __future__ import annotations

import argparse
import sys
import time
from dataclasses import dataclass
from typing import Iterable


DISPLAY_COUNT = 4
CHARACTERS_PER_DISPLAY = 8
FIFTEEN_SEGMENT_BITS = tuple(1 << bit for bit in range(15))


@dataclass(frozen=True)
class TestStep:
    description: str
    commands: tuple[str, ...]
    delay: float | None = None
    command_delay: float | None = None


def raw15_command(display: int, masks: Iterable[int]) -> str:
    """Build a raw command with tokens forced to the 15-segment form."""
    payload = ",".join(f"{mask & 0x7FFF:04X}" for mask in masks)
    return f"@R:{display}:{payload}"


def complex_raw_steps(display: int) -> Iterable[TestStep]:
    """Generate patterns that diagnose every segment and character position."""
    yield TestStep(
        f"display {display}: 15-segment walk, one segment on all characters",
        tuple(
            raw15_command(display, (segment,) * CHARACTERS_PER_DISPLAY)
            for segment in FIFTEEN_SEGMENT_BITS
        ),
        0.5,
        0.35,
    )

    yield TestStep(
        f"display {display}: fully lit character walking across all positions",
        tuple(
            raw15_command(
                display,
                (0x7FFF if position == active else 0 for position in range(CHARACTERS_PER_DISPLAY)),
            )
            for active in range(CHARACTERS_PER_DISPLAY)
        ),
        0.5,
        0.3,
    )

    cumulative_masks = tuple((1 << (index + 1)) - 1 for index in range(CHARACTERS_PER_DISPLAY))
    yield TestStep(
        f"display {display}: cumulative and mirrored 15-segment masks",
        (
            raw15_command(display, cumulative_masks),
            raw15_command(display, reversed(cumulative_masks)),
            raw15_command(display, (0x5555, 0x2AAA) * 4),
            raw15_command(display, (0x7FFF, 0x0000) * 4),
        ),
        0.8,
        0.7,
    )


def single_display_steps() -> Iterable[TestStep]:
    """Exercise every command form on every logical display."""
    seven_segment = "3F,06,5B,4F,66,6D,7D,07"
    wide_segment = "0001,0002,0004,0008,0010,0020,00C0,4000"

    for display in range(1, DISPLAY_COUNT + 1):
        yield TestStep(
            f"display {display}: normal text",
            (f"1:{display}:DISP {display}",),
        )
        yield TestStep(
            f"display {display}: numeric text with automatic decimal points",
            (f"1:{display}:12345678",),
        )
        yield TestStep(
            f"display {display}: flashing text",
            (f"2:{display}:FLASH {display}",),
            2.0,
        )
        yield TestStep(
            f"display {display}: match mode (last two characters blink)",
            (f"4:{display}:MATCH {display:02d}",),
            2.0,
        )
        yield TestStep(
            f"display {display}: 7-segment raw masks",
            (f"@R:{display}:{seven_segment}",),
        )
        yield TestStep(
            f"display {display}: 15-segment raw masks",
            (f"@R:{display}:{wide_segment}",),
        )
        yield from complex_raw_steps(display)
        yield TestStep(
            f"display {display}: brightness levels 0, 4, 8 and 15",
            tuple(f"@B:{display}:{level}" for level in (0, 4, 8, 15)),
            0.6,
            0.6,
        )
        yield TestStep(
            f"display {display}: legacy clear",
            (f"3:{display}:",),
        )
        yield TestStep(
            f"display {display}: extended clear",
            (f"1:{display}:CLEAR ME", f"@C:{display}"),
        )


def multi_display_steps() -> Iterable[TestStep]:
    """Exercise coordinated updates and commands with global scope."""
    yield TestStep(
        "all displays: different normal text sent as one command batch",
        tuple(f"1:{display}:DISPLAY{display}" for display in range(1, DISPLAY_COUNT + 1)),
    )
    yield TestStep(
        "all displays: different modes at the same time",
        (
            "1:1:NORMAL 1",
            "2:2:FLASH  2",
            "4:3:MATCH  03",
            "@R:4:0001,0002,0004,0008,0010,0020,00C0,4000",
        ),
        2.5,
    )
    yield TestStep(
        "all displays: coordinated counter-like values",
        tuple(f"1:{display}:{display * 11111111:08d}" for display in range(1, DISPLAY_COUNT + 1)),
    )
    yield TestStep(
        "all displays: coordinated 15-segment raw patterns",
        tuple(
            raw15_command(
                display,
                (
                    FIFTEEN_SEGMENT_BITS[
                        (position + display - 1) % len(FIFTEEN_SEGMENT_BITS)
                    ]
                    for position in range(CHARACTERS_PER_DISPLAY)
                ),
            )
            for display in range(1, DISPLAY_COUNT + 1)
        ),
        1.5,
    )
    yield TestStep(
        "all displays: complementary 15-segment checkerboards",
        tuple(
            raw15_command(
                display,
                (
                    0x5555 if (position + display) % 2 else 0x2AAA
                    for position in range(CHARACTERS_PER_DISPLAY)
                ),
            )
            for display in range(1, DISPLAY_COUNT + 1)
        ),
        1.5,
    )
    yield TestStep(
        "all displays: global brightness levels 0, 4, 8 and 15",
        tuple(f"@B:0:{level}" for level in (0, 4, 8, 15)),
        0.8,
        0.8,
    )
    yield TestStep(
        "all displays: global clear",
        ("@C:0",),
    )
    yield TestStep(
        "demo mode on and off (must run last)",
        ("@X:1", "@X:0"),
        0.5,
        5.0,
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Send a visual test sequence to the 15-segment controller."
    )
    parser.add_argument(
        "port",
        nargs="?",
        help="serial port, for example COM3 or /dev/ttyUSB0 (not needed with --dry-run)",
    )
    parser.add_argument("--baud", type=int, default=115200, help="UART baud rate")
    parser.add_argument(
        "--delay",
        type=float,
        default=1.2,
        help="default delay after a test step in seconds",
    )
    parser.add_argument(
        "--command-delay",
        type=float,
        default=0.15,
        help="delay between commands belonging to one step",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="print commands without opening a serial port or waiting",
    )
    parser.add_argument(
        "--interactive",
        action="store_true",
        help="wait for Enter after every test step",
    )
    parser.add_argument(
        "--only",
        choices=("single", "multi"),
        help="run only single-display or multi-display scenarios",
    )
    return parser.parse_args()


def build_steps(only: str | None) -> list[TestStep]:
    steps: list[TestStep] = []
    if only in (None, "single"):
        steps.extend(single_display_steps())
    if only in (None, "multi"):
        steps.extend(multi_display_steps())
    return steps


def main() -> int:
    args = parse_args()
    if not args.dry_run and not args.port:
        print("error: provide a serial port or use --dry-run", file=sys.stderr)
        return 2
    if args.delay < 0 or args.command_delay < 0:
        print("error: delays cannot be negative", file=sys.stderr)
        return 2

    serial_port = None
    if not args.dry_run:
        try:
            import serial
        except ImportError:
            print(
                "error: pyserial is required; install it with: "
                f"{sys.executable} -m pip install pyserial",
                file=sys.stderr,
            )
            return 2

        try:
            serial_port = serial.Serial(args.port, args.baud, timeout=1)
            time.sleep(2.0)  # Arduino Nano may reset when the port is opened.
        except serial.SerialException as exc:
            print(f"error: cannot open {args.port}: {exc}", file=sys.stderr)
            return 1

    steps = build_steps(args.only)
    try:
        for number, step in enumerate(steps, start=1):
            print(f"\n[{number:02d}/{len(steps):02d}] {step.description}")
            for command_number, command in enumerate(step.commands):
                print(f"  > {command}")
                if serial_port is not None:
                    serial_port.write((command + "\n").encode("ascii"))
                    serial_port.flush()
                if (
                    not args.dry_run
                    and command_number + 1 < len(step.commands)
                ):
                    between_commands = (
                        step.command_delay
                        if step.command_delay is not None
                        else args.command_delay
                    )
                    if between_commands:
                        time.sleep(between_commands)

            if args.dry_run:
                continue
            if args.interactive:
                input("  Press Enter to continue...")
            else:
                time.sleep(step.delay if step.delay is not None else args.delay)
    except KeyboardInterrupt:
        print("\nTest interrupted.")
        return 130
    finally:
        if serial_port is not None:
            serial_port.close()

    print("\nTest sequence completed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
