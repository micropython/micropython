# test "soft" machine.Timer (no hardware ID)
import sys

try:
    import time, machine

    machine.Timer
except:
    print("SKIP")
    raise SystemExit

if sys.platform in ("esp32", "esp8266"):
    print("SKIP")  # TODO: Implement soft timers for esp32/esp8266 ports
    raise SystemExit

# create and deinit
t = machine.Timer(freq=1)
t.deinit()

# deinit again
t.deinit()

# create 2 and deinit
t = machine.Timer(freq=1)
t2 = machine.Timer(freq=1)
t.deinit()
t2.deinit()

# create 2 and deinit in different order
t = machine.Timer(freq=1)
t2 = machine.Timer(freq=1)
t2.deinit()
t.deinit()

# create one-shot timer with callback and wait for it to print (should be just once)
t = machine.Timer(period=1, mode=machine.Timer.ONE_SHOT, callback=lambda t: print("one-shot"))
time.sleep_ms(5)
t.deinit()

# create periodic timer with callback and wait for it to print
t = machine.Timer(period=4, mode=machine.Timer.PERIODIC, callback=lambda t: print("periodic"))
time.sleep_ms(14)
t.deinit()
