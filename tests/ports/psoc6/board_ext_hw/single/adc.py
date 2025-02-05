### ADC Functional test
""" Setup description: 
    Construct a basic voltage divider with 120 ohms each resistor. Supply the ends with 3.3V and GND.
    Available voltage values are then - 3.3V, ~1.7V, 0V.
    Pin connections:
    Voltage Divider circuit     On Target Board
          3.3V end                adc_pin_max
          Mid point               adc_pin_mid
          GND end                 adc_pin_gnd 
    *Known issue: When connected to GND, you may not get exact 0V and this may vary board to board.
"""
import os
import time
from machine import ADC, ADCBlock

# Allocate pin based on board
board = os.uname().machine
if "CY8CPROTO-062-4343W" in board:
    gnd_tests_skip = False
    adc_pin_gnd = "P10_4"
    adc_pin_mid = "P10_3"
    adc_mid_chan = 3
    adc_pin_max = "P10_2"
    adc_wrong_pin_name = "P13_7"
elif "CY8CPROTO-063-BLE" in board:
    gnd_tests_skip = False
    adc_pin_gnd = "P10_2"
    adc_pin_mid = "P10_3"
    adc_mid_chan = 3
    adc_pin_max = "P10_4"
    adc_wrong_pin_name = "P13_7"
elif "CY8CKIT-062S2-AI" in board:
    gnd_tests_skip = True
    adc_pin_mid = "P10_0"
    adc_mid_chan = 0
    adc_pin_max = "P10_1"
    adc_wrong_pin_name = "P13_7"

# 0.35V
tolerance_uv = 350000

tolerance_raw = 4000

block = None


def validate_adc_uv_value(exp_volt, act_volt):
    if not (exp_volt - tolerance_uv) < act_volt < (exp_volt + tolerance_uv):
        print(
            "Expected voltage - ",
            exp_volt,
            "(uV) is approx same as obtained voltage(uV): ",
            act_volt,
        )


def validate_adc_raw_value(exp_volt, act_volt):
    if not (exp_volt - tolerance_raw) < act_volt < (exp_volt + tolerance_raw):
        print(
            "Expected voltage - ",
            exp_volt,
            "(raw) is approx same as obtained voltage(raw): ",
            act_volt,
        )


# Exception should be raised
try:
    adc = ADC(adc_wrong_pin_name)
except:
    print("Invalid ADC Pin\n")

block = ADCBlock(0, bits=12)
# ADCBlock.connect(channel)
adc1 = block.connect(3)
block.deinit()

# ADCBlock.connect(source)
block = ADCBlock(0, bits=12)
adc1 = block.connect(adc_pin_mid)
block.deinit()

if not gnd_tests_skip:
    adc0 = ADC(adc_pin_gnd, sample_ns=1000)

    adc0_value_uv = adc0.read_uv()
    validate_adc_uv_value(0, adc0_value_uv)
    adc0_value_raw = adc0.read_u16()
    validate_adc_raw_value(0, adc0_value_raw)

    adc0.deinit()

# ADCBlock.connect(channel,source)
block = ADCBlock(0, bits=12)
adc1 = block.connect(adc_mid_chan, adc_pin_mid)

adc2 = ADC(adc_pin_max, sample_ns=1000)

adc1_value_uv = adc1.read_uv()
validate_adc_uv_value(1650000, adc1_value_uv)
adc1_value_raw = adc1.read_u16()
validate_adc_raw_value(16385, adc1_value_raw)

adc2_value_uv = adc2.read_uv()
validate_adc_uv_value(3300000, adc2_value_uv)
adc2_value_raw = adc2.read_u16()
validate_adc_raw_value(32767, adc2_value_raw)


adc1.deinit()
adc2.deinit()
