# PWM test
"""
Setup: Connect pwm_pin to pin_in
"""
from machine import PWM, Pin
import os
import time

# Allocate pin based on board
board = os.uname().machine
if "CY8CPROTO-062-4343W" in board:
    pwm_pin = "P13_7"
    pin_in = "P13_6"
elif "CY8CPROTO-063-BLE" in board:
    pwm_pin = "P12_6"
    pin_in = "P12_7"
elif "CY8CKIT-062S2-AI" in board:
    pwm_pin = "P9_6"
    pin_in = "P9_7"

input_pin = Pin(pin_in, Pin.IN)


def measure_signal():
    # In some cases, the first measurement is not accurate
    # (negative time)
    # so we need to measure the signal until we get a positive timing
    positive_timing = False
    attempts = 0
    while not positive_timing and attempts < 3:
        init_level = input_pin.value()
        # Transient until edge transition
        # when the synch is done
        while input_pin.value() == init_level:
            pass

        # start measuring just after the edge transition
        init_level_start_time = time.ticks_us()

        # wait for edge transition
        while input_pin.value() == (not init_level):
            pass
        init_level_end_time = time.ticks_us()

        # wait for next edge transition
        while input_pin.value() == init_level:
            pass
        second_level_start_time = time.ticks_us()

        # Identify the low and high level timestamps
        if init_level == 1:
            low_level_start_time = init_level_start_time
            low_level_end_time = init_level_end_time
            high_level_start_time = init_level_end_time
            high_level_end_time = second_level_start_time
        else:
            low_level_start_time = init_level_end_time
            low_level_end_time = second_level_start_time
            high_level_start_time = init_level_start_time
            high_level_end_time = init_level_end_time

        on_time = time.ticks_diff(high_level_end_time, high_level_start_time)
        off_time = time.ticks_diff(low_level_end_time, low_level_start_time)
        time_period = on_time + off_time

        attempts += 1
        if time_period > 0:
            positive_timing = True

    # Calculate frequency and duty cycle
    calc_freq = 1000000 / (time_period)
    calc_dc = (on_time / time_period) * 100
    return calc_dc, calc_freq


def validate_signal(exp_freq=0, calc_freq=0, calc_dc=0, exp_dc=0, exp_duty_u16=0, exp_duty_ns=0):
    tolerance = 0.5
    duty_tolerance = 1.0

    set_freq = pwm.freq()
    set_duty_u16 = 0
    set_duty_ns = 0

    if exp_duty_ns:
        set_duty_ns = pwm.duty_ns()
    if exp_duty_u16:
        set_duty_u16 = pwm.duty_u16()

    if ((exp_freq - tolerance) < set_freq < (exp_freq + tolerance)) == False:
        print(
            f"Expected freq does not match set freq! \n Exp freq: {exp_freq} \n Set freq: {set_freq}"
        )

    if ((exp_freq - tolerance) < calc_freq < (exp_freq + tolerance)) == False:
        print(
            f"Expected freq does not match calc freq! \n Exp freq: {exp_freq} \n Set freq: {calc_freq}"
        )
    # This one is failing intermittently on the CI/CD
    if ((exp_dc - duty_tolerance) < calc_dc < (exp_dc + duty_tolerance)) == False:
        print(f"Exp dc(%) does not match calc dc(%)! \n Exp dc: {exp_dc} \n Calc dc: {calc_dc}")

    if set_duty_ns != 0:
        if set_duty_ns != exp_duty_ns:
            print(
                f"Exp dc(ns) does not match set dc(ns) \n Exp dc: {exp_duty_ns} \n Set dc: {set_duty_ns}"
            )

    if set_duty_u16 != 0:
        if set_duty_u16 != exp_duty_u16:
            print(
                f"Exp dc(raw) does not match set dc(raw) \n Exp dc: {exp_duty_u16} \n Set dc: {set_duty_u16}"
            )


print("*** PWM tests ***")
# T = 1sec (25% dc)
pwm = PWM(pwm_pin, freq=1, duty_ns=250000000)
time.sleep(2)  # Wait for the pwm signal to be initialized and started
calc_dc, calc_freq = measure_signal()
validate_signal(
    exp_freq=1,
    calc_freq=calc_freq,
    exp_dc=25,
    calc_dc=calc_dc,
    exp_duty_u16=0,
    exp_duty_ns=250000000,
)

# T = 1sec (50% dc)
pwm.duty_ns(500000000)
calc_dc, calc_freq = measure_signal()
validate_signal(
    exp_freq=1,
    calc_freq=calc_freq,
    exp_dc=50,
    calc_dc=calc_dc,
    exp_duty_u16=0,
    exp_duty_ns=500000000,
)

# T = 1sec (75% dc)
pwm.duty_u16(49151)
calc_dc, calc_freq = measure_signal()
validate_signal(
    exp_freq=1, calc_freq=calc_freq, exp_dc=75, calc_dc=calc_dc, exp_duty_u16=49151, exp_duty_ns=0
)

# Reconfigure frequency and dutycycle
# T = 0.5 sec (50% dc)
pwm.init(freq=2, duty_u16=32767)
calc_dc, calc_freq = measure_signal()
validate_signal(
    exp_freq=2, calc_freq=calc_freq, exp_dc=50, calc_dc=calc_dc, exp_duty_u16=32767, exp_duty_ns=0
)

pwm.deinit()
input_pin.deinit()
