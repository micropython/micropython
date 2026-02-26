# Wiznet W55RP20-EVB-Pico

## Network Example

To use network / socket based code, connect ethernet port to network with DHCP running:

```
>>> import network
>>> from machine import Pin, WIZNET_PIO_SPI
>>> spi = WIZNET_PIO_SPI(baudrate=10_000_000, sck=Pin(21), cs=Pin(20), mosi=Pin(23), miso=Pin(22))
>>> print(spi)
WIZNET_PIO_SPI(baudrate=10416666, sck=Pin(21), cs=Pin(20), mosi=Pin(23), miso=Pin(22))
>>> nic = network.WIZNET6K(spi, Pin(20), Pin(25))  # spi, cs, reset pin
>>> nic.active(True)
>>> nic.ifconfig()
('0.0.0.0', '0.0.0.0', '0.0.0.0', '0.0.0.0')
>>> nic.ifconfig("dhcp")
('192.168.0.10', '255.255.255.0', '192.168.0.1', '192.168.0.1')
>>> nic.isconnected()
True
```
At this point standard network communications libraries should work.