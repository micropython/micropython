# Arduino Nano 33 BLE and Nano 33 BLE Sense

The [Arduino Nano 33 BLE](https://store.arduino.cc/usa/nano-33-ble-with-headers) and
[Arduino Nano 33 BLE Sense](https://store.arduino.cc/usa/nano-33-ble-sense) and
are built around the NINA B306 module, based on Nordic nRF 52840 and containing
a powerful Cortex M4F. Both include an onboard 9 axis Inertial Measurement Unit (IMU), the LSM9DS1.
The Nano 33 BLE Sense adds an LPS22HB barometric pressure and temperature sensor,
an ADPS-9960 digital proximity, ambient light, RGB, and gensture sensor,
and an MP34DT05 digital microphone.

Note: the Arduino Nano 33 BLE and BLE Sense do not include a QSPI external
flash. Any Python code will need to be stored on the internal flash
filesystem.

I2C pins `board.SCL1` and `board.SDA1` are not exposed and are used for onboard peripherals.
Pin `board.R_PULLUP` must be set to high to enable the `SCL1` and `SDA1` pullups for proper operation.

Pin `board.VDD_ENV` applies power to the LSM9DS1, and must be high for it to be operational.

Pins `board.MIC_PWR`, `board.PDMDIN`, and `board.PDMCLK` are for the Nano 33 BLE Sense onboard microphone.

Pin `board.INT_ADPS` is the interrupt pin from the ADPS-9960.

Pins `board.RGB_LED_R`, `board.RGB_LED_G`, and `board.RGB_LED_B`
are the red, green and blue LEDS in the onboard RGB LED.

Pins `board.LED_G` and `board.LED_Y` are onboard green and red LEDs. `board.LED_Y` is also `board.SCK`.
