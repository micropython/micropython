# PWM test
"""
Setup: Connect pwm_pin to pin_in
"""
from machine import PWM, Pin
import os
import time

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pwm_pin = "P13_7"
    pin_in = "P13_6"
    duty_tolerance = 10.0  # Different per board to accommodate HIL limitations

elif "CY8CPROTO-063-BLE" in machine:
    pwm_pin = "P12_6"
    pin_in = "P12_7"
    duty_tolerance = 50.0

input_pin = Pin(pin_in, Pin.IN)

start_time = 0
low_signal_start_time = 0
high_signal_start_time = 0
tolerance = 3.0
debug = False


def _print_val(params_list, print_list=False):
    if print_list:
        for params in params_list:
            print(f"{params[0]} = {params[1]}")


# def measure_signal_recursive():
#     global start_time
#     if input_pin.value() == 1:
#         start_time = time.ticks_us()
#         wait_for_low()
#         return
#     measure_signal_recursive()


# def wait_for_low():
#     global low_signal_start_time
#     while input_pin.value():
#         pass
#     low_signal_start_time = time.ticks_us()
#     wait_for_high()


# def wait_for_high():
#     global high_signal_start_time
#     while input_pin.value() < 1:
#         pass
#     high_signal_start_time = time.ticks_us()


def measure_signal():
    global start_time
    global low_signal_start_time
    global high_signal_start_time

    while input_pin.value() == 0:
        pass
    start_time = time.ticks_us()
    # wait for low
    while input_pin.value():
        pass
    low_signal_start_time = time.ticks_us()
    # wait for high
    while input_pin.value() < 1:
        pass
    high_signal_start_time = time.ticks_us()


def validate_signal(exp_freq=0, exp_duty_u16=0, exp_duty_ns=0, exp_dutycycle=0):
    on_time = time.ticks_diff(low_signal_start_time, start_time)
    off_time = time.ticks_diff(high_signal_start_time, low_signal_start_time)
    time_period = on_time + off_time
    freq = 1000000 / (time_period)
    dc = on_time / (time.ticks_diff(high_signal_start_time, start_time)) * 100

    params = [
        ["on_time(us)", on_time],
        ["off_time(us)", off_time],
        ["time_period(us)", time_period],
        ["freq(Hz)", freq],
        ["dc(%)", dc],
    ]
    _print_val(params, debug)

    print(
        "\nExpected freq(Hz) approx same as set freq(Hz) and experimentally calculated freq(Hz): ",
        (exp_freq - tolerance) < pwm.freq() < (exp_freq + tolerance)
        and (exp_freq - tolerance) < freq < (exp_freq + tolerance),
    )

    if exp_duty_ns:
        print("Dutycycle(ns) value is same as set value: ", pwm.duty_ns() == exp_duty_ns)
    if exp_duty_u16:
        print("Dutycycle(raw) value is same as set value: ", pwm.duty_u16() == exp_duty_u16)

    print(
        "Expected duty cycle(%) approx same as experimental duty cycle(%): ",
        (exp_dutycycle - duty_tolerance) < dc < (exp_dutycycle + duty_tolerance),
    )


# T = 1sec (25% dc)
pwm = PWM(pwm_pin, freq=1, duty_ns=250000000)
# Let the first pulse pass
time.sleep(1)
print(
    "\nTest Case 1: \n freq(Hz): ",
    pwm.freq(),
    ", duty_on(ns): ",
    pwm.duty_ns(),
    ", dutycycle(%): 25%",
)
measure_signal()
validate_signal(exp_freq=1, exp_duty_u16=0, exp_duty_ns=250000000, exp_dutycycle=25)

# T = 1sec (50% dc)
pwm.duty_ns(500000000)
# Let the first pulse pass
time.sleep(1)
print(
    "\nTest Case 2: \n freq(Hz): ",
    pwm.freq(),
    ", duty_on(ns): ",
    pwm.duty_ns(),
    ", dutycycle(%): 50%",
)
measure_signal()
validate_signal(exp_freq=1, exp_duty_u16=0, exp_duty_ns=500000000, exp_dutycycle=50)

# T = 1sec (75% dc)
pwm.duty_u16(49151)
# Let the first pulse pass
time.sleep(1)
print(
    "\nTest Case 3: \n freq(Hz): ",
    pwm.freq(),
    ", duty_u16(raw): ",
    pwm.duty_u16(),
    ", dutycycle(%): 75%",
)
measure_signal()
validate_signal(exp_freq=1, exp_duty_u16=49151, exp_duty_ns=0, exp_dutycycle=75)

# Reconfigure frequency and dutycycle T = 1sec (50% dc)
pwm.init(freq=2, duty_u16=32767)
# Let the first 2 pulses pass
time.sleep(1)
print(
    "\nTest Case 4: \n freq(Hz): ",
    pwm.freq(),
    ", duty_u16(raw): ",
    pwm.duty_u16(),
    ", dutycycle(%): 50%",
)
measure_signal()
validate_signal(exp_freq=2, exp_duty_u16=32767, exp_duty_ns=0, exp_dutycycle=50)

pwm.deinit()
