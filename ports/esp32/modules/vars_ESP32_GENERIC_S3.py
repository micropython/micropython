#### COPYRIGHT AMAS TECHNOLOGIES ####
## LICENSED UNDER AGPL-3.0 ##
#### COPYRIGHT AMAS TECHNOLOGIES ####


from machine import Pin, SoftI2C, ADC


# Ambient Temperature Humidity Sensor
amb_scl = 46; amb_sda = 18
# Water Level Sensor
analog_water_sensor = ADC(Pin(1))
analog_water_sensor.atten(ADC.ATTN_11DB)
water_power = Pin(2, Pin.OUT)
# Buttons
hard_reset_button = Pin(4, Pin.IN, Pin.PULL_UP)
network_reset_button = Pin(5, Pin.IN, Pin.PULL_UP)
# Relays
pump = Pin(38, Pin.OUT)
light = Pin(48, Pin.OUT)
# Error light
err_light = Pin(6, Pin.OUT)
# Data Light
dat_light = Pin(7, Pin.OUT)
# RTC
i2c_r = SoftI2C(Pin(46, Pin.OPEN_DRAIN), Pin(18, Pin.OPEN_DRAIN))