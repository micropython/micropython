# Arduino Nano 33 BLE Rev2 and Nano 33 BLE Sense Rev2

The [Arduino Nano 33 BLE Rev2](https://store.arduino.cc/usa/nano-33-ble-rev2) and
[Arduino Nano 33 BLE Sense Rev2](https://store.arduino.cc/usa/nano-33-ble-sense-rev2)
are built around the NINA-B306 module, based on the Nordic nRF52840 and containing
a powerful Cortex-M4F. Both include an onboard 9-axis Inertial Measurement Unit (IMU), made up of the BMI270 6-axis IMU and the BMM150 3-axis magnetometer.
The Nano 33 BLE Sense Rev2 adds an LPS22HB barometric pressure and temperature sensor, an HS3003 humidity sensor,
an APDS-9960 digital proximity, ambient light, RGB, and gesture sensor,
and an MP34DT06JTR digital microphone.

Note: the Arduino Nano 33 BLE Rev2 and BLE Sense Rev2 do not include a QSPI external
flash. Any Python code will need to be stored on the internal flash
filesystem.

I2C pins `board.SCL1` and `board.SDA1` are not exposed and are used for onboard peripherals.
Pin `board.R_PULLUP` must be set to high to enable the `SCL1` and `SDA1` pullups for proper operation.

Pin `board.VDD_ENV` applies power to the BMI270, the BMM150, the LPS22HB and the HS3003, and must be high for them to be operational.

Pins `board.MIC_PWR`, `board.PDMDIN`, and `board.PDMCLK` are for the Nano 33 BLE Sense onboard microphone.

Pin `board.INT_APDS` is the interrupt pin from the APDS-9960.

Pins `board.INT_BMI_1` and `board.INT_BMI_2` are the two interrupt pins from the BMI270.

Pin `board.INT_LPS` is the interrupt pin from the LPS22.

Pins `board.RGB_LED_R`, `board.RGB_LED_G`, and `board.RGB_LED_B`
are the red, green and blue LEDS in the onboard RGB LED.

Pins `board.LED_G` and `board.LED_Y` are onboard green and red LEDs. `board.LED_Y` is also `board.SCK`.
