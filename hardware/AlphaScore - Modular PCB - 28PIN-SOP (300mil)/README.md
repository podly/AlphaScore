# AlphaScore - Modular PCB - 28PIN-SOP (300mil)

## Bill of Materials (BOM)

| Quantity | Component | Specification / notes |
|---------:|-----------|-----------------------|
| 1 | Arduino Nano | ATmega328P-based module |
| 1 | 10 kΩ THT trimmer potentiometer | 5 × 5 mm pin spacing |
| 1 | 470 µF / 10 V THT electrolytic capacitor | 3.5 mm lead spacing |
| 1 | 2-pin connector | 2.54 mm pitch, e.g. Molex KK-254 |
| 9 | 5-pin connector | 2.54 mm pitch, e.g. Molex KK-254; see the connector notes below |
| 8 | HT16K33 | SOP28 package (300 mil) |
| 16 | 5241A dual-character 15-segment LED display | 0.54-inch character height, common cathode |
| 8 | 1N4148 diode | SOD-323 package / footprint |

### 0805 SMD components

| Quantity | Component |
|---------:|-----------|
| 24 | 39 kΩ resistor |
| 16 | 4.7 kΩ resistor |
| 8 | 10 µF capacitor |
| 9 | 100 nF capacitor |

### 5-pin connector notes

The 5-pin connectors may be either horizontal or vertical and can be mounted on
the front or back of the PCB, depending on the installation requirements. Only
four pins are electrically connected; the fifth pin is unused.

The second connector does not need to be fitted to the final display board in
the chain. In this configuration, only eight 5-pin connectors are required.

Ready-made 5-pin KK-254 wire harnesses can be used to connect the modules. Make
sure that the wires are arranged in the correct order at both connectors before
powering the system.
