# PWM test
"""
Setup: Connect pwm_pin to pin_in
"""

from machine import PWM, Pin, Timer
import os
import time

pwm_pin = "P16_1"
pin_in = "P16_0"

input_pin = Pin(pin_in, Pin.IN)


def timer_noop_cb(t):
    pass


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

#### freq getter / setter tests ####

# freq_get returns the frequency set at construction (duty_u16)
pwm = PWM(pwm_pin, freq=1000, duty_u16=32767)
print(f"freq_get after init (duty_u16): {pwm.freq()}")
pwm.deinit()

# freq_get returns the frequency set at construction (duty_ns)
pwm = PWM(pwm_pin, freq=500, duty_ns=1000000)
print(f"freq_get after init (duty_ns): {pwm.freq()}")
pwm.deinit()

# freq_set changes output frequency (duty_u16, measured on hardware)
pwm = PWM(pwm_pin, freq=10, duty_u16=32767)
pwm.freq(50)
calc_dc, calc_freq = measure_signal()
validate_signal(exp_freq=50, calc_freq=calc_freq, exp_dc=50, calc_dc=calc_dc)
pwm.deinit()

# freq_set changes output frequency (duty_ns, measured on hardware)
# duty_ns=2500000: 25% at 100 Hz -> 50% at 200 Hz (period shrinks from 10ms to 5ms)
pwm = PWM(pwm_pin, freq=100, duty_ns=2500000)
pwm.freq(200)
calc_dc, calc_freq = measure_signal()
validate_signal(exp_freq=200, calc_freq=calc_freq, exp_dc=50, calc_dc=calc_dc)
pwm.deinit()

# freq_get after freq_set returns the new frequency
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
pwm.freq(250)
print(f"freq_get after freq_set: {pwm.freq()}")
pwm.deinit()

#### Negative freq_set tests ####

# freq_set: frequency must be greater than 0
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
try:
    pwm.freq(0)
except Exception as e:
    print(e)
pwm.deinit()

# freq_set: negative frequency
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
try:
    pwm.freq(-1)
except Exception as e:
    print(e)
pwm.deinit()

# freq_set: frequency exceeds maximum
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
try:
    pwm.freq(500001)
except Exception as e:
    print(e)
pwm.deinit()

# freq_set: duty_ns would exceed new (lower) period
pwm = PWM(pwm_pin, freq=100, duty_ns=9000000)  # 9 ms on-time, period = 10 ms
try:
    pwm.freq(200)  # new period = 5 ms; 9 ms duty exceeds it
except Exception as e:
    print(e)
pwm.deinit()

#### duty_u16 getter / setter tests ####

# duty_u16_get returns value set at construction
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
print(f"duty_u16_get after init: {pwm.duty_u16()}")
pwm.deinit()

# duty_u16_get converts duty_ns to u16
# duty_ns=5000000 at 100 Hz = 50% -> u16 = 32767
pwm = PWM(pwm_pin, freq=100, duty_ns=5000000)
print(f"duty_u16_get from duty_ns: {pwm.duty_u16()}")
pwm.deinit()

# duty_u16_set changes output duty cycle (measured on hardware)
# 16383 / 65536 ~= 25%
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
pwm.duty_u16(16383)
calc_dc, calc_freq = measure_signal()
validate_signal(exp_freq=100, calc_freq=calc_freq, exp_dc=25, calc_dc=calc_dc)
pwm.deinit()

# duty_u16_get after duty_u16_set returns the new value
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
pwm.duty_u16(16383)
print(f"duty_u16_get after duty_u16_set: {pwm.duty_u16()}")
pwm.deinit()

#### duty_ns getter / setter tests ####

# duty_ns_get returns value set at construction
pwm = PWM(pwm_pin, freq=100, duty_ns=2500000)
print(f"duty_ns_get after init: {pwm.duty_ns()}")
pwm.deinit()

# duty_ns_get converts duty_u16 to ns
# duty_u16=32767 at 100 Hz: (32767+1)/65536 * 10000000 = 5000000 ns
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
print(f"duty_ns_get from duty_u16: {pwm.duty_ns()}")
pwm.deinit()

# duty_ns_set changes output duty cycle (measured on hardware)
# 2500000 ns at 100 Hz = 25%
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
pwm.duty_ns(2500000)
calc_dc, calc_freq = measure_signal()
validate_signal(exp_freq=100, calc_freq=calc_freq, exp_dc=25, calc_dc=calc_dc)
pwm.deinit()

# duty_ns_get after duty_ns_set returns the new value
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
pwm.duty_ns(2500000)
print(f"duty_ns_get after duty_ns_set: {pwm.duty_ns()}")
pwm.deinit()

#### Negative duty setter tests ####

# duty_ns_set: duty exceeds period (period = 10 ms = 10000000 ns at 100 Hz)
pwm = PWM(pwm_pin, freq=100, duty_u16=32767)
try:
    pwm.duty_ns(15000000)
except Exception as e:
    print(e)
pwm.deinit()

# A new PWM wants to use a TCPWM instance used by another PWM in the same pin.
pwm_owner = PWM(pwm_pin, freq=1, duty_u16=32767)
time.sleep(2)  # Wait for the pwm signal to be initialized and started
try:
    pwm_conflict = PWM(pwm_pin, freq=1, duty_u16=32767)
except Exception as e:
    print(f"pwm_vs_pwm_same_pin_conflict: {e}")
finally:
    pwm_owner.deinit()

# A new PWM wants to use a TCPWM instance used by a Timer.
# P16_2 uses TCPWM0 counter 2, which is also used by Timer(0).
timer_owner = Timer(0, period=10000, mode=Timer.PERIODIC, callback=timer_noop_cb)
try:
    pwm_conflict = PWM("P16_2", freq=100, duty_u16=32767)
except Exception as e:
    print(f"pwm_vs_timer_conflict: {e}")
else:
    # With fallback allocation enabled, PWM should use another available LINE counter.
    print("pwm_vs_timer_conflict: no conflict (fallback counter allocated)")
    pwm_conflict.deinit()
finally:
    timer_owner.deinit()

# A new PWM wants to use a TCPWM instance used by Timers, and all fallback
# counters on the pin are already occupied.
# P16_2 exposes LINE counters 2 and 258. Reserve both via Timer(0) and Timer(6).
timer_owner_a = Timer(0, period=10000, mode=Timer.PERIODIC, callback=timer_noop_cb)
timer_owner_b = Timer(6, period=10, mode=Timer.PERIODIC, callback=timer_noop_cb)
try:
    pwm_conflict = PWM("P16_2", freq=100, duty_u16=32767)
except Exception as e:
    print(f"pwm_vs_timer_conflict_all_counters: {e}")
else:
    print("pwm_vs_timer_conflict_all_counters: no conflict (unexpected)")
    pwm_conflict.deinit()
finally:
    timer_owner_b.deinit()
    timer_owner_a.deinit()
