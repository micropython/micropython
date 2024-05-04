from machine import Pin, ADC


READ_BAT = Pin.board.READ_BAT

ADC0_BAT = Pin.board.ADC0_BAT

HICHG = Pin.board.P13

READ_BAT.value(0)

adc = ADC(ADC0_BAT)

bled = Pin(6)

# bled.value(0)


# NRF_SAADC_GAIN1_4 
# NRF_SAADC_REFERENCE_VDD4 VDD/4 as reference.

def battery_volts():
    _in = adc.value()
    return _in/255.0 * 0.825 * 2.9608 * 4.0