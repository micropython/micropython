# Test machine.Counter.
#
# IMPORTANT: This test requires hardware connections: the pulse-outputs and pulse-inputs
# pins must be wired together (OUT_PIN1 to INP_PIN1 and OUT_PIN2 to INP_PIN2).

import sys

try:
    from machine import Counter, Pin, PWM, Timer
except ImportError:
    print("SKIP")
    raise SystemExit


FREQ = 500_000  # 500  # Hz
FILTER_NS = 25  # ns
SECONDS = 1
MATCH = FREQ * SECONDS  # 2 seconds counting time
# MATCH = 75
MATCH_1000 = round(MATCH / 1000 * 3)  # match margin per thousand
if MATCH_1000 == 0:
    MATCH_1000 = 1

print(f"MATCH:{MATCH}, MATCH_1000:{MATCH_1000}")
print()

# Configure pins based on the target.
if "esp32" in sys.platform:
    OUT_PIN1, INP_PIN1 = 4, 5

    OUT_PIN2, INP_PIN2 = 16, 17
elif "esp8266" in sys.platform:
    #     OUT_PIN1, INP_PIN1 = 4, 5
    #
    #     OUT_PIN2, INP_PIN2 = 16, 17
    pass
elif "mimxrt" in sys.platform:
    #     OUT_PIN1 = "D0"
    #     INP_PIN1 = "D1"
    #
    #     OUT_PIN2 = "D2"
    #     INP_PIN2 = "D3"
    pass
elif "rp2" in sys.platform:
    #     OUT_PIN1 = "GPIO0"
    #     INP_PIN1 = "GPIO1"
    #
    #     OUT_PIN2 = "GPIO2"
    #     INP_PIN2 = "GPIO3"
    pass
elif "samd" in sys.platform:
    #     OUT_PIN1 = "D0"
    #     INP_PIN1 = "D1"
    #
    #     OUT_PIN2 = "D2"
    #     INP_PIN2 = "D3"
    pass
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


def toggle_pin(pin, x):
    print("Toggle", pin, x, "time(s).")
    start_value = pin()
    for _ in range(x):
        pin(not pin())
    print(" value:", pin, start_value, "->", pin())


is_callback = False
callback_counter_value = 0
pwm1 = None
counter = None


@micropython.viper
def callback_print(counter):
    global is_callback, callback_counter_value
    is_callback = True
    callback_counter_value = counter.value()
    print(f"  callback Сounter value:{callback_counter_value:10} status:{counter.status():3}")


@micropython.viper
def callback_pwm_deinit(counter):
    pwm1.deinit()
    print("  callback pwm.deinit()")
    callback_print(counter)


@micropython.viper
def callback_timer(timer):
    global callback_counter_value
    pwm1.deinit()
    print("  callback pwm.deinit()")
    callback_counter_value = counter.value()
    print(f"  callback Сounter value:{callback_counter_value:10} status:{counter.status():3}")


try:
    print("Test the wire connections: output1 -> input1.")
    output1 = Pin(OUT_PIN1, mode=Pin.OUT, value=1)
    input1 = Pin(INP_PIN1)
    print("output1->input1:", output1, output1(), "->", input1, input1())
    assert output1() == 1
    assert input1() == 1

    output1(0)
    print("output1->input1:", output1, output1(), "->", input1, input1())
    assert output1() == 0
    assert input1() == 0
    print("Ok.")
    print()

    print("Test the wire connections: output2 -> input2.")
    output2 = Pin(OUT_PIN2, mode=Pin.OUT, value=1)
    input2 = Pin(INP_PIN2)
    print("output2->input2:", output2, output2(), "->", input2, input2())
    assert output2() == 1
    assert input2() == 1

    output2(0)
    print("output2->input2:", output2, output2(), "->", input2, input2())
    assert output2() == 0
    assert input2() == 0
    print("Ok.")
    print()

    print("# ====================================================================================")
    print("1) Count pulses on 'src' Pin() if 'direction' is a constant.")
    print(f"'src': 'output1' {output1} -> 'input1' {input1}")

    print("# ------------------------------------------------------------------------------------")
    print(f"1.1) Count pulses 'output1' {output1} UP at the RISING edge.")
    # counter = Counter(0, input1, direction=Counter.UP, edge=Counter.RISING)
    counter = Counter(0, input1)  # the same
    print(counter, "value:", counter.value())
    assert counter.value() == 0
    toggle_pin(output1, 1)
    print(counter, "value:", counter.value())
    assert counter.value() == 1
    toggle_pin(output1, 1)
    print(counter, "value:", counter.value())
    assert counter.value() == 1
    print("Ok.")
    print()

    print("# ------------------------------------------------------------------------------------")
    print(f"1.2) Count pulses 'output1' {output1} DOWN at the FALLING edge.")
    counter.init(input1, direction=Counter.DOWN, edge=Counter.FALLING)
    print(counter, "value:", counter.value())
    assert counter.value() == 0
    toggle_pin(output1, 1)
    print(counter, "value:", counter.value())
    assert counter.value() == 0
    toggle_pin(output1, 1)
    print(counter, "value:", counter.value())
    assert counter.value() == -1
    print("Ok.")
    print()
    counter.deinit()

    print("# ====================================================================================")
    print("2) Count pulses on 'src' Pin() if 'direction' is a Pin().")
    print(f"'output1' {output1} -> 'input1' {input1} is 'src' of counted pulses.")
    print(f"'output2' {output2} -> 'input2' {input2} is 'direction' of counting.")
    print("# ------------------------------------------------------------------------------------")
    output2(0)
    print(
        f"2.1) Count pulses 'output1' {output1} DOWN when 'direction='{output2} is {output2()} at RISING-FALLING edges."
    )
    counter = Counter(0, src=input1, direction=input2, edge=Counter.RISING | Counter.FALLING)
    print(f"'src': 'output1' {output1} -> 'input1' {input1}")
    print("'direction':", output2, output2(), "->", input2, input2())
    print(counter, "value:", counter.value())
    assert counter.value() == 0
    toggle_pin(output1, 1)
    print(counter, "value:", counter.value())
    assert counter.value() == -1
    toggle_pin(output1, 1)
    print(counter, "value:", counter.value())
    assert counter.value() == -2
    print("Ok.")
    print()
    print("# ------------------------------------------------------------------------------------")
    output2(1)
    print(
        f"2.2) Count pulses 'output1' {output1} UP when 'direction='{output2} is {output2()} at RISING-FALLING edges."
    )
    print(f"'src': 'output1' {output1} -> 'input1' {input1}")
    print("'direction':", output2, output2(), "->", input2, input2())
    toggle_pin(output1, 1)
    print(counter, "value:", counter.value())
    assert counter.value() == -1
    toggle_pin(output1, 1)
    print(counter, "value:", counter.value())
    assert counter.value() == 0
    print("Ok.")
    print()

    print("# ====================================================================================")
    print(
        "3) Count pulses on 'src' Pin() UP to (2^16 + 1000) and DOWN to -(2^16 + 1000) and callbacks."
    )
    print(f"'output1' {output1} -> 'input1' {input1} is 'src' of counted pulses.")
    print(f"'output2' {output2} -> 'input2' {input2} is 'direction' of counting.")
    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_print, trigger=Counter.IRQ_MATCH, value=1_000)
    if "esp32" in sys.platform:
        counter.irq(handler=callback_print, trigger=Counter.IRQ_ZERO)
    assert counter.value() == 0

    print("# ------------------------------------------------------------------------------------")
    output2(1)
    print(f"3.1) Count pulses 'output1' {output1} UP to (2^16 + 1000).")
    toggle_pin(output1, 2**16 + 1000)
    print(counter, "value:", counter.value(), "dif:", counter.value() - 2**16 + 1000)
    assert counter.value() == 2**16 + 1000, 2**16 + 1000
    assert is_callback == True
    print("Ok.")
    print()

    print("# ------------------------------------------------------------------------------------")
    output2(0)
    print(f"3.2) Count pulses 'output1' {output1} DOWN to -(2^16 + 1000).")
    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_print, trigger=Counter.IRQ_MATCH, value=-1_000)
    toggle_pin(output1, 2 * (2**16 + 1000))
    print(counter, "value:", counter.value(), "dif:", counter.value() + 2**16 + 1000)
    assert counter.value() == -(2**16 + 1000), -(2**16 + 1000)
    assert is_callback == True
    print("Ok.")
    print()
    counter.deinit()

    print("# ====================================================================================")
    print("4) Count pulses UP on 'src' Pin() and DOWN on '_src' Pin().")
    print("'output1' {output1} -> 'input1' {input1} is 'src' counted UP pulses.")
    print("'output2' {output2} -> 'input2' {input2} is '_src' counted DOWN pulses.")

    print("# ------------------------------------------------------------------------------------")
    print(
        f"4.1) Count pulses 'output1' {output1} UP on 'src' input1 {output1} at RISING-FALLING edges."
    )
    counter = Counter(0, src=input1, _src=input2, edge=Counter.RISING | Counter.FALLING)
    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_print, trigger=Counter.IRQ_MATCH, value=1_000)
    if "esp32" in sys.platform:
        counter.irq(handler=callback_print, trigger=Counter.IRQ_ZERO)
    print(counter, "value:", counter.value())
    toggle_pin(output1, 1_100)
    print(counter, "value:", counter.value(), "dif:", counter.value() - 1_100)
    assert counter.value() == 1_100
    assert is_callback == True
    print("Ok.")
    print()

    print("# ------------------------------------------------------------------------------------")
    print(
        f"4.2) Count pulses 'output1' {output1} DOWN on '_src' input2 {output2} at RISING-FALLING edges."
    )
    toggle_pin(output2, 2 * 1_100)
    print(counter, "value:", counter.value(), "dif:", counter.value() + 1_100)
    assert counter.value() == -1_100
    assert is_callback == True
    print("Ok.")
    print()
    counter.irq(handler=None, trigger=Counter.IRQ_MATCH)
    if "esp32" in sys.platform:
        counter.irq(handler=None, trigger=Counter.IRQ_ZERO)
    counter.deinit()

    print("# ====================================================================================")
    pwm1 = PWM(output1, freq=FREQ, duty_u16=32768)
    pwm1.deinit()
    output2(1)
    print("5) Testing the count of PWM pulses on 'src' Pin() if 'direction' is a Pin().")
    print("   Stop the PWM in Timer callbacks.")
    print(f"'output1' {pwm1} -> 'input1' {input1} is 'src' of counted PWM pulses.")
    print(f"'output2' {output2} -> 'input2' {input2} is 'direction' of counting.")
    print("# ------------------------------------------------------------------------------------")
    output2(1)
    print(
        f"5.1) Count pulses 'output1' {pwm1} UP when 'direction='{output2} is {output2()} at FALLING edges."
    )
    counter = Counter(0, src=input1, direction=input2, edge=Counter.FALLING)
    print(counter, "value:", counter.value())
    timer = Timer(1)
    timer.init(
        mode=Timer.ONE_SHOT, period=1_000 * SECONDS + 2 * MATCH_1000, callback=callback_timer
    )
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() < MATCH:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() - MATCH)
    assert counter.value() >= MATCH
    assert counter.value() < MATCH + MATCH_1000
    print("Ok.")
    print()
    timer.deinit()

    print("# ------------------------------------------------------------------------------------")
    output2(0)
    print(
        f"5.2) Count pulses 'output1' {output1} DOWN when 'direction='{output2} is {output2()} at FALLING edges."
    )
    print(counter, "value:", counter.value())
    timer = Timer(1)
    timer.init(
        mode=Timer.ONE_SHOT, period=2 * 1_000 * SECONDS + 2 * MATCH_1000, callback=callback_timer
    )
    print(timer)
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() > -MATCH:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() + MATCH)
    assert counter.value() <= -MATCH
    assert counter.value() > -MATCH - MATCH_1000
    print("Ok.")
    print()
    counter.deinit()
    timer.deinit()
    pwm1.deinit()

    print("# ====================================================================================")
    print("6) Testing the count of PWM pulses on 'src' Pin() if 'direction' is a Pin().")
    print("   Stop PWM in Counter callback.")
    print(f"'output1' {pwm1} -> 'input1' {input1} is 'src' of counted PWM pulses.")
    print(f"'output2' {output2} -> 'input2' {input2} is 'direction' of counting.")
    print("# ------------------------------------------------------------------------------------")
    output2(1)
    print(
        f"6.1) Count pulses 'output1' {pwm1} UP when 'direction='{output2} is {output2()} at RISING edges."
    )

    counter1 = Counter(1, src=input1, direction=input2, filter_ns=FILTER_NS)
    counter = Counter(0, src=input1, direction=input2, filter_ns=FILTER_NS)
    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=MATCH)
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() < MATCH:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() - MATCH)
    assert counter.value() >= MATCH
    assert counter.value() < MATCH + MATCH_1000
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=MATCH + 300)
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() < MATCH + 300:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() - (MATCH + 300))
    assert counter.value() >= MATCH + 300
    assert counter.value() < MATCH + 300 + MATCH_1000
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=MATCH + 500)
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() < MATCH + 500:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() - (MATCH + 500))
    assert counter.value() >= MATCH + 500
    assert counter.value() < MATCH + 500 + MATCH_1000
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    print("# ------------------------------------------------------------------------------------")
    output2(0)
    print(
        f"6.2) Count pulses 'output1' {pwm1} DOWN when 'direction='{output2} is {output2()} at RISING edges."
    )

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=MATCH + 300)
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() > MATCH + 300:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() - (MATCH + 300))
    assert counter.value() <= MATCH + 300
    assert counter.value() > MATCH + 300 - MATCH_1000
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=MATCH)
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() > MATCH:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() - MATCH)
    assert counter.value() <= MATCH
    assert counter.value() > MATCH - MATCH_1000
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=0)
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() > 0:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() - 0)
    assert counter.value() <= 0
    assert counter.value() > -MATCH_1000
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    counter.irq(handler=None, trigger=Counter.IRQ_MATCH)

    #     is_callback = False
    #     callback_counter_value = counter.value()
    #     if "esp32" in sys.platform:
    #         counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_ZERO)
    #     print(counter, "value:", counter.value())
    #     pwm1.init(freq=FREQ, duty_u16=32768)
    #     while counter.value() > 0:
    #         pass
    #     print(counter, "value:", counter.value(), "dif:", counter.value() - 0)
    #     assert counter.value() <= 0
    #     assert counter.value() > -MATCH_1000
    #     assert counter.value() == counter1.value()
    #     assert is_callback == True
    #     print("Ok.")
    #     print()

    if "esp32" in sys.platform:
        counter.irq(handler=None, trigger=Counter.IRQ_ZERO)

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=-MATCH)
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() > -MATCH:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() + MATCH)
    assert counter.value() <= -MATCH
    assert counter.value() > -(MATCH + MATCH_1000)
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=-(MATCH + 300))
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() > -(MATCH + 300):
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() + MATCH + 300)
    assert counter.value() <= -(MATCH + 300)
    assert counter.value() > -(MATCH + 300 + MATCH_1000)
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=-(MATCH + 500))
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() > -(MATCH + 500):
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() + MATCH + 500)
    assert counter.value() <= -(MATCH + 500)
    assert counter.value() > -(MATCH + 500 + MATCH_1000)
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    print("# ------------------------------------------------------------------------------------")
    output2(1)
    print(
        f"6.3) Count pulses 'output1' {pwm1} UP when 'direction='{output2} is {output2()} at RISING edges."
    )

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=-(MATCH + 300))
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() < -(MATCH + 300):
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() + MATCH + 300)
    assert counter.value() >= -(MATCH + 300)
    assert counter.value() < -(MATCH + 300 - MATCH_1000)
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    is_callback = False
    callback_counter_value = counter.value()
    counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=-MATCH)
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() < -MATCH:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() + MATCH)
    assert counter.value() >= -MATCH
    assert counter.value() < -(MATCH - MATCH_1000)
    assert counter.value() == counter1.value()
    assert is_callback == True
    print("Ok.")
    print()

    counter.irq(handler=None, trigger=Counter.IRQ_MATCH)

    is_callback = False
    callback_counter_value = counter.value()
    if "esp32" in sys.platform:
        counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_ZERO)
    else:
        counter.irq(handler=callback_pwm_deinit, trigger=Counter.IRQ_MATCH, value=0)
    print(counter, "value:", counter.value())
    pwm1.init(freq=FREQ, duty_u16=32768)
    while counter.value() < 0:
        pass
    print(counter, "value:", counter.value(), "dif:", counter.value() - 0)
    assert counter.value() >= 0
    assert counter.value() < MATCH_1000
    # assert counter.value() == counter1.value()
    # assert is_callback == True
    print("Ok.")
    print()

    print("END.")

finally:
    print("finally:")
    try:
        pwm1.deinit()
    except:
        pass
    try:
        print(counter1, "value:", counter1.value())
    except:
        pass
    try:
        counter1.deinit()
    except:
        pass
    try:
        print(counter, "value:", counter.value())
    except:
        pass
    try:
        counter.deinit()
    except:
        pass
