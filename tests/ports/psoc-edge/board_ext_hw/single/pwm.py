# PWM test
"""
Setup: Connect pwm_pin to pin_in
"""

from machine import PWM, Pin
import os
import time

pwm_pin = "P16_1"
pin_in = "P16_0"

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
    frequency_tolerance_percent = 10
    duty_tolerance = 1.0

    if (
        (exp_freq - exp_freq * frequency_tolerance_percent / 100)
        < calc_freq
        < (exp_freq + exp_freq * frequency_tolerance_percent / 100)
    ) == False:
        print(
            f"Expected freq does not match calc freq! \n Exp freq: {exp_freq} \n Calc freq: {calc_freq}"
        )
    else:
        print(f"Expected freq matches calc freq! \n freq: {exp_freq}")

    if ((exp_dc - duty_tolerance) < calc_dc < (exp_dc + duty_tolerance)) == False:
        print(f"Exp dc(%) does not match calc dc(%)! \n dc: {exp_dc} \n Calc dc: {calc_dc}")
    else:
        print(f"Exp dc(%) matches calc dc(%)! \n dc: {exp_dc}")


print("*** PWM tests ***")
# T = 1sec (50% dc)
pwm = PWM(pwm_pin, freq=1, duty_u16=32767)
time.sleep(2)  # Wait for the pwm signal to be initialized and started
calc_dc, calc_freq = measure_signal()

validate_signal(
    exp_freq=1,
    calc_freq=calc_freq,
    exp_dc=50,
    calc_dc=calc_dc,
    exp_duty_u16=32767,
    exp_duty_ns=0,
)

# Reconfigure frequency and dutycycle
pwm.init(freq=10, duty_u16=49151)
calc_dc, calc_freq = measure_signal()
validate_signal(
    exp_freq=10, calc_freq=calc_freq, exp_dc=75, calc_dc=calc_dc, exp_duty_u16=49151, exp_duty_ns=0
)

# Reconfigure frequency and dutycycle
pwm.init(freq=100, duty_ns=2500000)
calc_dc, calc_freq = measure_signal()
validate_signal(
    exp_freq=100,
    calc_freq=calc_freq,
    exp_dc=25,
    calc_dc=calc_dc,
    exp_duty_u16=0,
    exp_duty_ns=2500000,
)
pwm.deinit()

#### Negative test cases ####

# PWM frequency out of range
try:
    pwm = PWM(pwm_pin, freq=500001, duty_u16=32767)
except Exception as e:
    print(e)

# PWM frequency must be greater than 0
try:
    pwm = PWM(pwm_pin, freq=0, duty_u16=32767)
except Exception as e:
    print(e)

# PWM duty_ns larger than period (20,000,000 ns)
try:
    pwm = PWM(pwm_pin, freq=50, duty_ns=25000000)
except Exception as e:
    print(e)

# PWM duty cycle only one format should be specified
try:
    pwm = PWM(pwm_pin, freq=10, duty_u16=32767, duty_ns=100000)
except Exception as e:
    print(e)

# Negative frequency
try:
    pwm = PWM(pwm_pin, freq=-1, duty_u16=32767)
except Exception as e:
    print(e)

# No duty specified
try:
    pwm = PWM(pwm_pin, freq=100)
except Exception as e:
    print(e)

# Non-PWM-capable pin
try:
    pwm = PWM("P0_0", freq=100, duty_u16=32767)
except Exception as e:
    print(e)

# PWM instance already exists
pwm = PWM(pwm_pin, freq=1, duty_u16=32767)
time.sleep(2)  # Wait for the pwm signal to be initialized and started
try:
    pwm2 = PWM(pwm_pin, freq=1, duty_u16=32767)
except Exception as e:
    print(e)
    pwm.deinit()
