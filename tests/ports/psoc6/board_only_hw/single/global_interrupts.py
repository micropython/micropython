from machine import Timer
import machine
import time

oneshot_triggered = False


def callback_oneshot(timer):
    global oneshot_triggered
    oneshot_triggered = True


def test_oneshot():
    # Oneshot timer
    global oneshot_triggered
    tim_oneshot = Timer(0, period=1, mode=Timer.ONE_SHOT, callback=callback_oneshot)

    try:
        # Wait for 5 seconds
        for i in range(5):
            time.sleep(1)
            if oneshot_triggered:
                print("Interrupt triggered")
                oneshot_triggered = False
    finally:
        tim_oneshot.deinit()  # Deinitialize the Oneshot timer


state = machine.disable_irq()
print("*****Test 1: Interrupts Disabled*****")
test_oneshot()
machine.enable_irq(state)
print("*****Test 2: Interrupts Enabled******")
test_oneshot()
