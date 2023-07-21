# B-L072Z-LRWAN1 Board Notes

## LoRa on the B-L072Z-LRWAN1

The [B-L072Z-LRWAN1 Discovery kit](https://www.st.com/en/evaluation-tools/b-l072z-lrwan1.html)
can be used for applications involving LoRa communications. The heart of the
board in a Murata CMWX1ZZABZ-091 module which contains an STM32L072CZ and a
Semtech SX1276 LoRa transceiver.

This MicroPython board definition is built to target the STM32L0 - and the
micropython-lib [lora library](https://github.com/micropython/micropython-lib/tree/master/micropython/lora)
provides LoRa functionality using the SX1276.

### LoRa library installation

See the [installation instructions](https://github.com/micropython/micropython-lib/tree/master/micropython/lora#installation)
for the lora library. In short, `lora-sx127x` and either `lora-sync` or
`lora-async` will need to be installed using `mpremote` and `mip`.

```bash
mpremote mip install lora-sync
mpremote mip install lora-sx127x
```

See [Package management](https://docs.micropython.org/en/latest/reference/packages.html)
for more information on installation with `mip` and `mpremote`.

### Initialising the LoRa modem

`SPI(1)` is configured appropriately for the connection between the micro and
the LoRa radio and, where appropriate, pins have useful aliases. An example of
modem initialisation:

```python
from machine import Pin, SPI
# SX_SCK, SX_MOSI, SX_MISO are the default SPI1 pins  
modem = SX1276(
    spi=SPI(1, baudrate=2000_000, polarity=0, phase=0),
    cs = Pin.board.SX_CS,
    dio0=Pin.board.SX_DIO0,
    dio1=Pin.board.SX_DIO1,
    reset=Pin.board.SX_RESET,
    lora_cfg=lora_cfg,
)
```
