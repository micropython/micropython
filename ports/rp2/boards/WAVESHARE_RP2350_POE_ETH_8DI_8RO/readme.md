# Waveshare RP2350-POE-ETH-8DI-8RO

The onboard W5500 is configured automatically, so Ethernet can be started with:

```python
import network

nic = network.WIZNET5K()
nic.active(True)
```

The board aliases in `Pin.board` include the relay outputs (`RELAY1` through
`RELAY8`), isolated digital inputs (`DI1` through `DI8`), RTC, RS485, SD-card,
and W5500 signals.
