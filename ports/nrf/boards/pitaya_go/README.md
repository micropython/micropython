# Makerdiary Pitaya Go

>Pitaya Go is a compact and versatile development platform for IoT solutions,
combining the Nordic's high-end multiprotocol SoC nRF52840 and the Microchip's
extreme low power Wi-Fi® network controller ATWINC1500B. It offers a complete
solution for wireless connectivity with IEEE 802.11 b/g/n, Bluetooth 5, Thread
and Zigbee, that is specifically designed for the IoT.
Pitaya Go features a Battery Charger with power path management, 64Mbit ultra
low power QSPI Flash memory, additional NFC-A Tag PCB Antenna, user
programmable RGB LED and Buttons, reversible USB-C Connector and easily
expandable Header Sockets. All these features above make this board an ideal
choice for the next IoT project.

from [Makerdiary](https://store.makerdiary.com/products/pitaya-go)


## Installing CircuitPython submodules

Before you can build, you will need to run the following commands once, which
will install the submodules that are part of the CircuitPython ecosystem, and
build the `mpy-cross` tool:

```
$ cd circuitpython
$ git submodule update --init
$ make -C mpy-cross
```


## Building
```sh
$ cd ports/nrf
$ make BOARD=pitaya_go SD=s140 -V=1 -j4
```

# Flashing CircuitPython

The Pitaya Go has a pre-programmed bootloader which can be used to program the
Pitaya Go. Follow [the guide - How to Program Pitaya Go](https://wiki.makerdiary.com/pitaya-go/programming/)
to flash the CircuitPython firmware.
