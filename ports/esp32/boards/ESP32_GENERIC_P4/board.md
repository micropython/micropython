The following firmware is applicable to most development boards based on ESP32-P4, and
the development boards must be equipped with at least 16 MiB external SPI Flash.

The P4 chip had major changes at chip revision 3.0, and these require that builds
for earlier (`PRE_REV3`) chips are done as variants.

Espressif denote the revision 3+ chips by appending an **`X`** to the part numbers:

|part|revisions 0.x and 1.x|revisions 3.x and later|
|----|---------------------|-----------------------|
|ESP32-P4 16Mb PSRAM|ESP32-P4NRW16|ESP32-P4NRW16**X**|
|ESP32-P4 32Mb PSRAM|ESP32-P4NRW32|ESP32-P4NRW32**X**|
|Multimedia DEVBOARD|ESP32-P4-Function-EV-Board|ESP32-P4**X**-Function-EV-Board|

etc..

This board also has variants available to select the Wireless CoProcessor (if any):

* If your board has a standalone revision 3.x or later ESP32-P4 processor (or the board has a coprocessor but you do not want to use it) then choose the generic variant (first heading below).
* If you have an early (revisions 0.x and 1.x) chip revision choose the "PRE REV3" variants.
* If your board has an external ESP32-C5 coprocessor for WiFi and BLE then choose the
  "C5 WiFi/BLE" variants.
* If your board has an external ESP32-C6 coprocessor for WiFi and BLE then choose the
  "C6 WiFi/BLE" variants.
