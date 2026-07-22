# AlphaScore Firmware

The firmware runs on an ATmega328P-based Arduino Nano and controls eight
HT16K33/VK16K33 drivers over I2C. It receives myPinballs-compatible and extended
AlphaScore commands over UART, renders text and raw segment data, controls
brightness and blink modes, and provides a built-in Demo Mode.

The project uses PlatformIO and supports Arduino Nano boards with both the new
and old bootloaders. From the repository root, build both variants with:

```bash
pio run --project-dir firmware -e nano-new-bootloader -e nano-old-bootloader
```

## Compile-time configuration

Firmware behaviour and hardware assignments can be adjusted in
[`src/Config.h`](src/Config.h). Rebuild and upload the firmware after changing
these values.

| Setting | Default | Description |
|---------|---------|-------------|
| `SerialBaud` | `115200` | UART communication speed. |
| `LogicalDisplays` | `4` | Number of logical displays. |
| `DriversPerDisplay` | `2` | Number of display drivers assigned to each logical display. |
| `CharactersPerDriver` | `4` | Number of characters handled by one driver. |
| `Vt16k33BaseAddress` | `0x70` | First I2C address in the consecutive driver-address range. |
| `BrightnessPotPin` | `A0` | Analog input used by the brightness potentiometer. |
| `BrightnessMin` / `BrightnessMax` | `0` / `15` | Available hardware brightness range. |
| `DefaultBrightness` | `4` | Brightness applied when the display drivers are initialized. |
| `BrightnessUpdateMs` | `100` ms | Potentiometer sampling interval when hardware brightness control is enabled by JP24. |
| `Jp24Pin`–`Jp27Pin` | `2`–`5` | Arduino inputs used by the Bright, F1, F2 and F3/Demo jumpers. |
| `BlinkOnMs` | `500` ms | Duration of the visible phase in blinking modes. |
| `BlinkOffMs` | `200` ms | Duration of the hidden phase in blinking modes. |
| `SelfTestEnabled` | `true` | Enables or disables the startup display self-test. |
| `SelfTestMessageMs` | `3000` ms | Duration of the startup self-test message. |
| `ReverseDigitOrder` | `true` | Reverses the logical-to-physical character order to match the PCB layout. |

The `ModeNormal`, `ModeFlash`, `ModeClear`, `ModeMatch` and `ModeRaw` constants
define the internal mode identifiers used by the serial protocol and normally
should not be changed.

## Utilities

* [`test_serial_protocol.py`](test_serial_protocol.py) runs the display and
  protocol test sequence.
* [`demo_mode.py`](demo_mode.py) enables or disables Demo Mode over UART.
* [`reference/old_arduino_code/`](reference/old_arduino_code/) contains the
  legacy firmware for reference.
