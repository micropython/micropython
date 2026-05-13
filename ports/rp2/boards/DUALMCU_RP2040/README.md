# UNIT Electronics DualMCU RP2040 + ESP32

Board definition for the RP2040 side of the DualMCU board.

## Default buses

- LED: GPIO25
- UART0: TX=GPIO0, RX=GPIO1
- UART1: TX=GPIO8, RX=GPIO9
- I2C0: SDA=GPIO12, SCL=GPIO13
- SPI0: SCK=GPIO18, MOSI=GPIO19, MISO=GPIO20, CS=GPIO21

## Build

```bash
make -C mpy-cross
make -C ports/rp2 BOARD=DUALMCU_RP2040 submodules
make -C ports/rp2 BOARD=DUALMCU_RP2040
```
