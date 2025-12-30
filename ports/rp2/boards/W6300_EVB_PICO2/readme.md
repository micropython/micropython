# Wiznet W6300-EVB-Pico2

## Network Example

To use network / socket based code, connect ethernet port to network with DHCP running:

```
>>> import network
>>> from machine import Pin, WIZNET_PIO_SPI
>>> spi = WIZNET_PIO_SPI(baudrate=40_000_000, sck=Pin(17), cs=Pin(16), io0=Pin(18), io1=Pin(19), io2=Pin(20), io3=Pin(21))
>>> print(spi)
WIZNET_PIO_SPI(baudrate=50000000, mode=QUAD, sck=Pin(17), cs=Pin(16), io0=Pin(18), io1=Pin(19), io2=Pin(20), io3=Pin(21))
>>> nic = network.WIZNET6K(spi, Pin(16), Pin(22))  # spi, cs, reset pin
>>> nic.active(True)
>>> nic.ifconfig()
('0.0.0.0', '0.0.0.0', '0.0.0.0', '0.0.0.0')
>>> nic.ifconfig("dhcp")
('192.168.0.10', '255.255.255.0', '192.168.0.1', '192.168.0.1')
>>> nic.isconnected()
True
```
At this point standard network communications libraries should work.
