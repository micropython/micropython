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
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    adc_pin_gnd = "P10_1"
    adc_pin_mid = "P10_3"
    adc_pin_max = "P10_0"
    adc_wrong_pin_name = "P13_7"
    # Enable after DUT setup
    # print("SKIP")
    # raise SystemExit
elif "CY8CPROTO-063-BLE" in machine:
    adc_pin_gnd = "P10_2"
    adc_pin_mid = "P10_3"
    adc_pin_max = "P10_4"
    adc_wrong_pin_name = "P13_7"
    # print("SKIP")
    # raise SystemExit

# 0.35V
tolerance_uv = 350000

tolerance_raw = 4000

block = None


def validate_adc_uv_value(adc_pin, exp_volt, act_volt):
    print(
        "Expected voltage - ",
        exp_volt,
        "(uV) is approx same as obtained voltage(uV): ",
        (exp_volt - tolerance_uv) < act_volt < (exp_volt + tolerance_uv),
    )


def validate_adc_raw_value(adc_pin, exp_volt, act_volt):
    print(
        "Expected voltage - ",
        exp_volt,
        "(raw) is approx same as obtained voltage(raw): ",
        (exp_volt - tolerance_raw) < act_volt < (exp_volt + tolerance_raw),
    )


# Exception should be raised
try:
    adc = ADC(adc_wrong_pin_name)
except:
    print("Invalid ADC Pin\n")

try:
    adcBlock = ADCBlock(1)
except:
    print("TypeError: Specified ADC id not supported. Currently only block 0 is configured!")

try:
    adcBlock = ADCBlock(0, bits=10)
except:
    print("TypeError: Invalid bits. Current ADC configuration supports only 12 bits resolution!")


block = ADCBlock(0, bits=12)
# ADCBlock.connect(channel)
adc1 = block.connect(3)
block.deinit()

# ADCBlock.connect(source)
block = ADCBlock(0, bits=12)
adc1 = block.connect(adc_pin_mid)
block.deinit()

adc0 = ADC(adc_pin_gnd, sample_ns=1000)

# ADCBlock.connect(channel,source)
block = ADCBlock(0, bits=12)
adc1 = block.connect(3, adc_pin_mid)

adc2 = ADC(adc_pin_max, sample_ns=1000)

adc0_value_uv = adc0.read_uv()
validate_adc_uv_value(adc_pin_gnd, 0, adc0_value_uv)
adc0_value_raw = adc0.read_u16()
validate_adc_raw_value(adc_pin_gnd, 0, adc0_value_raw)

adc1_value_uv = adc1.read_uv()
validate_adc_uv_value(adc_pin_mid, 1650000, adc1_value_uv)
adc1_value_raw = adc1.read_u16()
validate_adc_raw_value(adc_pin_mid, 16385, adc1_value_raw)

adc2_value_uv = adc2.read_uv()
validate_adc_uv_value(adc_pin_max, 3300000, adc2_value_uv)
adc2_value_raw = adc2.read_u16()
validate_adc_raw_value(adc_pin_max, 32767, adc2_value_raw)

adc0.deinit()
adc1.deinit()
adc2.deinit()
