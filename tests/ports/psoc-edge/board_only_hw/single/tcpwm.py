from machine import PWM, Timer


def timer_noop_cb(timer):
    pass


print("*****TCPWM Ownership/Conflict Tests*****")

# A new PWM wants to use a TCPWM instance used by another PWM on the same pin.
pwm_owner = PWM("P16_1", freq=1, duty_u16=32767)
try:
    pwm_conflict = PWM("P16_1", freq=1, duty_u16=32767)
except Exception as e:
    print("pwm_vs_pwm_same_pin_conflict:", e)
finally:
    pwm_owner.deinit()

# A new PWM wants to use a TCPWM instance used by a Timer.
# P16_2 uses TCPWM0 counter 2, which is now used by Timer(2).
timer_owner = Timer(2, period=10000, mode=Timer.PERIODIC, callback=timer_noop_cb)
try:
    pwm_conflict = PWM("P16_2", freq=100, duty_u16=32767)
except Exception as e:
    print("pwm_vs_timer_conflict:", e)
else:
    # With fallback allocation enabled, PWM should use another available LINE counter.
    print("pwm_vs_timer_conflict: no conflict (fallback counter allocated)")
    pwm_conflict.deinit()
finally:
    timer_owner.deinit()

# A new PWM wants to use a TCPWM instance used by Timers, and all fallback
# counters on the pin are already occupied.
# P16_2 exposes LINE counters 2 and 258. Reserve both via Timer(2) and Timer(10).
timer_owner_a = Timer(2, period=10000, mode=Timer.PERIODIC, callback=timer_noop_cb)
timer_owner_b = Timer(10, period=10, mode=Timer.PERIODIC, callback=timer_noop_cb)
try:
    pwm_conflict = PWM("P16_2", freq=100, duty_u16=32767)
except Exception as e:
    print("pwm_vs_timer_conflict_all_counters:", e)
else:
    print("pwm_vs_timer_conflict_all_counters: no conflict (unexpected)")
    pwm_conflict.deinit()
finally:
    timer_owner_b.deinit()
    timer_owner_a.deinit()

# Constructor check for Timer/PWM ownership conflict on the same TCPWM instance.
pwm_owner = PWM("P16_2", freq=100, duty_u16=32767)
try:
    try:
        Timer(2, period=1000, mode=Timer.ONE_SHOT, callback=timer_noop_cb)
    except ValueError as e:
        print("timer_vs_pwm_conflict:", e)
finally:
    pwm_owner.deinit()
