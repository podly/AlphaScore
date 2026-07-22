<p align="center">
  <h1 align="center">AlphaScore 15-segment Pinball Display</h1>

  <p align="center">
    A complete hardware and firmware solution for four 8-character alphanumeric pinball displays.
  </p>
</p>

<p align="center">
  <img
    src="assets/images/score-board-15-segments-long.png"
    alt="AlphaScore 15-segment pinball display system"
    width="1200"
  >
</p>

---

## Overview

AlphaScore is a complete 15-segment display solution for pinball
machines. It combines 2-character LED display modules, HT16K33/VK16K33 display
drivers, an Arduino Nano controller, PCB designs, firmware and serial protocol
documentation into one project. A complete system drives up to four independent
8-character alphanumeric displays (32 characters in total).

The hardware is available in three PCB versions: two integrated **Wide PCB**
variants for different HT16K33 packages and a **Modular PCB** solution with a
separate Arduino Nano controller and display modules. Schematics, PCB gerber files,
renders, assembly information and a detailed comparison are available in the
[hardware documentation](hardware/README.md).

AlphaScore was designed as a replacement display system for classic pinball
machines. It is fully compatible with the **myPinballs Compatibility Protocol**
and also provides an extended protocol for direct segment control, brightness
adjustment, display management and additional functions. The host communicates
with the Arduino Nano over UART, while the Arduino controls the eight display
drivers over I2C.

---

## Features

* Complete 15-segment display hardware and firmware solution
* Up to 4 independent 8-character alphanumeric displays
* 32 individually controlled character positions in total
* Three PCB implementations: two integrated Wide variants and one Modular variant
* 2-character common-cathode LED modules in multiple colours
* Eight HT16K33 or compatible VK16K33 display drivers
* Arduino Nano (ATmega328P) controller
* UART communication (115200 baud)
* I2C communication between the Arduino and display drivers
* Compatible with the myPinballs Compatibility Protocol
* Extended AlphaScore Protocol
* Direct 7-segment and 15-segment control
* Individual or global brightness control
* Demo Mode
* Automatic thousands separator formatting for numeric values

---

## Project contents

* [`hardware/`](hardware/) — three PCB solutions, schematics, gerber files and board renders;
* [`firmware/`](firmware/) — Arduino Nano firmware, build configuration and test utilities;
* [`protocol/`](protocol/) — complete myPinballs and AlphaScore serial protocol documentation;
* [`docs/`](docs/) — component datasheets and supporting technical documentation;
* [`assets/`](assets/) — images used by the project documentation.

---

## Protocol documentation

AlphaScore supports the **myPinballs Compatibility Protocol** and the extended
**AlphaScore Protocol**. Their complete command reference, communication
parameters and examples are available in [`protocol/README.md`](protocol/README.md).

---

## Hardware

The AlphaScore display system is built around:

* Arduino Nano (ATmega328P), with firmware variants for both the **old** and
  **new** bootloader
* 2-character, 15-segment common-cathode displays from the
  [ELD-5241A-B series](docs/datasheets/ELD-5241A-B/ELD-5241A-B.pdf), for example:
  * **5241AW** (white)
  * **5241AB** (blue)
  * **5241AS** (red)
  * **5241AY** (yellow)
* [**HT16K33**](docs/datasheets/HT16K33/HT16K33Av111.pdf) or **VK16K33** display drivers

Three PCB implementations are included:

* [**AlphaScore - Wide PCB - 28PIN-SSOP (150mil)**](hardware/AlphaScore%20-%20Wide%20PCB%20-%2028PIN-SSOP%20%28150mil%29/) — integrated wide board using the smaller SSOP28 package;
* [**AlphaScore - Wide PCB - 28PIN-SOP (300mil)**](hardware/AlphaScore%20-%20Wide%20PCB%20-%2028PIN-SOP%20%28300mil%29/) — integrated wide board using the larger SOP28 package;
* [**AlphaScore - Modular PCB - 28PIN-SOP (300mil)**](hardware/AlphaScore%20-%20Modular%20PCB%20-%2028PIN-SOP%20%28300mil%29/) — separate Arduino Nano controller and HT16K33 display-module PCBs.

Detailed differences, assembly notes and the contents of each hardware directory
are described in [`hardware/README.md`](hardware/README.md).

The Arduino communicates with the display drivers over the **I2C** bus. The
controller uses eight drivers, and each driver must have a unique address set
with its solder jumpers. Configure the drivers with consecutive addresses from
`0x70` through `0x77`.

Prebuilt firmware variants for ATmega328P boards with the old and new Arduino
Nano bootloaders are automatically attached to every GitHub Release.

### Configuration jumpers

The controller provides four active-low configuration jumpers. A jumper is
active when it is closed and pulls the corresponding Arduino input to ground.
Their states are read during startup, so restart the controller after changing
a jumper.

| Jumper | PCB label | Function | Description |
|--------|-----------|----------|-------------|
| **JP24** | **Bright** | Brightness | Enables hardware brightness control using the onboard potentiometer. While active, serial `@B` brightness commands are ignored. When inactive, the potentiometer is not read and brightness is controlled through `@B` commands. |
| **JP25** | **F1** | Reserved | Reserved for future use. It currently has no effect. |
| **JP26** | **F2** | Fancy font | Enables the myPinballs-compatible Fancy font. When inactive, the Standard font is used. |
| **JP27** | **F3** or **Demo** | Demo Mode | Starts the controller in Demo Mode. While Demo Mode is active, display commands are ignored except `@X`, which can be used to disable it. |

---

## Prebuilt firmware

Ready-to-flash `.hex` files for Arduino Nano boards with both the new and old
bootloaders are available on the
[GitHub Releases page](https://github.com/podly/AlphaScore/releases). Download
the file matching the bootloader installed on your board.

---

## Building the firmware

The firmware is built with [PlatformIO](https://platformio.org/) and requires
Python 3. Install the PlatformIO command-line tools with:

```bash
python -m pip install --upgrade platformio
```

From the repository root, build both supported Arduino Nano variants:

```bash
pio run --project-dir firmware -e nano-new-bootloader -e nano-old-bootloader
```

To build only one variant, omit the other `-e` option. The generated firmware
files are written to:

* `firmware/.pio/build/nano-new-bootloader/firmware.hex`
* `firmware/.pio/build/nano-old-bootloader/firmware.hex`

Choose `nano-new-bootloader` for current Arduino Nano boards and
`nano-old-bootloader` for boards using the legacy bootloader. To compile and
upload a selected variant directly to a connected board, run:

```bash
pio run --project-dir firmware -e nano-new-bootloader --target upload --upload-port COM3
```

Replace `COM3` with the board's serial port, for example `/dev/ttyUSB0` on
Linux, and change the environment name if the board uses the old bootloader.

---

## Testing

The complete display system can be verified with the included
[`firmware/test_serial_protocol.py`](firmware/test_serial_protocol.py) script.
It sends a visual test sequence that exercises all controller functions on every
display, including text modes, individual segments and character positions,
brightness, clearing and Demo Mode.

Install the required serial library and run the script with the controller's
serial port:

```bash
python -m pip install pyserial
python firmware/test_serial_protocol.py COM3
```

Replace `COM3` with the appropriate port, for example `/dev/ttyUSB0` on Linux.
Use `--interactive` to confirm each test step manually or `--dry-run` to preview
the commands without connecting to the controller.

---

## License

The firmware and software tools are licensed under the MIT License.
See [LICENSE](LICENSE).

Hardware documentation is distributed under separate terms.
See [hardware/LICENSE.md](hardware/LICENSE.md).
