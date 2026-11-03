The LILYGO T-2CAN is an ESP32-S3 board with two isolated CAN channels.

- **CAN-B** is driven by the ESP32-S3 native TWAI peripheral (TX = GPIO7,
  RX = GPIO6) through an on-board isolated transceiver, and is usable directly
  via `machine.CAN`.
- **CAN-A** is an SPI MCP2515 (classic SKU) / MCP2518FD (CAN-FD SKU) controller.
  It is not driven by the ESP32 TWAI peripheral and needs a separate MCP25xx
  driver.

Example using the native CAN-B channel:

```python
from machine import CAN

can = CAN(1, 500_000)        # 500 kbit/s on CAN-B (GPIO7/GPIO6)
can.set_filters(None)        # accept all incoming messages
can.send(0x123, b"hello")
msg = can.recv()
print(msg)
```
