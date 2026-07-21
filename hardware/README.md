# AlphaScore Hardware

This directory contains three alternative PCB implementations of the AlphaScore
display controller. They provide the same basic display functionality but use
different PCB layouts and HT16K33 packages.

## Available PCB variants

| Variant | HT16K33 package | Construction | Recommended when |
| --- | --- | --- | --- |
| [AlphaScore - Wide PCB - 28PIN-SSOP (150mil)](AlphaScore%20-%20Wide%20PCB%20-%2028PIN-SSOP%20%28150mil%29/) | SSOP28, 150 mil | Integrated wide PCB | A compact HT16K33 footprint is preferred |
| [AlphaScore - Wide PCB - 28PIN-SOP (300mil)](AlphaScore%20-%20Wide%20PCB%20-%2028PIN-SOP%20%28300mil%29/) | SOP28, 300 mil | Integrated wide PCB | The larger package is easier to source or assemble |
| [AlphaScore - Modular PCB - 28PIN-SOP (300mil)](AlphaScore%20-%20Modular%20PCB%20-%2028PIN-SOP%20%28300mil%29/) | SOP28, 300 mil | Separate controller and display-module PCBs | A flexible, serviceable modular installation is required |

## Wide PCB variants

The two Wide PCB designs differ primarily in the footprint used for the
HT16K33 display-driver IC:

* **28PIN-SSOP (150mil)** uses the smaller SSOP28 package.
* **28PIN-SOP (300mil)** uses the larger SOP28 package.

Both are integrated wide-board solutions. Select the design that matches the
exact package of the HT16K33 devices being fitted. The SSOP28 and SOP28
footprints differ in body width and pin pitch and are **not interchangeable**.

Each variant directory contains front and back PCB renders together with its
schematic PDF.

## Modular PCB variant

The Modular PCB solution separates the system into:

* a controller PCB containing the Arduino Nano and the controller circuitry;
* separate LED display-module PCBs, each using an HT16K33 in the SOP28
  (300 mil) package.

This arrangement makes it possible to position display modules independently
and replace or service an individual module without replacing one large PCB.
The modular directory contains renders and separate schematic PDFs for the
controller and display module.

## Displays and I2C addressing

The boards are intended for 2-character, 15-segment common-cathode displays,
such as 5241AW (white), 5241AB (blue), 5241AS (red) and 5241AY (yellow).
HT16K33 or compatible VK16K33 drivers communicate with the Arduino Nano over
the I2C bus.

AlphaScore uses eight display drivers. Every driver on the shared I2C bus must
have a unique address configured with its solder jumpers. Set the addresses
consecutively from `0x70` through `0x77`; duplicated addresses will cause bus
conflicts and incorrect display operation.

## Assembly notes

Before ordering or assembling a board:

* verify that the PCB variant matches the physical HT16K33 package;
* verify display polarity and pinout—the design requires common-cathode
  displays with the expected footprint;
* configure and inspect all address solder jumpers before powering the full
  set of boards;
* check component orientation, solder bridges and supply polarity;
* review the schematic PDF and PCB renders stored in the selected variant
  directory.

When using the modular design, keep I2C and power connections short and secure.
All modules must share ground with the Arduino Nano. Check the supply capacity
for the complete display assembly before connecting power.

