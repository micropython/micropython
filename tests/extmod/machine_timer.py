import sys

try:
    from machine import Timer
    from time import sleep_ms
except:
    print("SKIP")
    raise SystemExit

if sys.platform in ("nrf"):
    # The nrf port doesn't support selection of hard and soft callbacks,
    # and only allows Timer(period=N), not Timer(freq=N).
    print("SKIP")
    raise SystemExit
elif sys.platform in ("esp32", "esp8266"):
    # Test with a hardware timer as software timers aren't implemented
    # on the esp32 and esp8266 ports. Timer ID 0 is guaranteed to exist.
    timer_id = 0
else:
    timer_id = -1

# Test both hard and soft IRQ handlers and both one-shot and periodic
# timers. We adjust period in tests/extmod/machine_soft_timer.py, so try
# adjusting freq here instead. The heap should be locked in hard callbacks
# and unlocked in soft callbacks.


def callback(t):
    print("callback", modes[mode], kinds[hard], freq, end=" ")
    try:
        allocate = bytearray(1)
        print("unlocked")
    except MemoryError:
        print("locked")


modes = {Timer.ONE_SHOT: "one-shot", Timer.PERIODIC: "periodic"}
kinds = {False: "soft", True: "hard"}

for mode in modes:
    for hard in kinds:
        for freq in 500, 250:
            timer = Timer(
                timer_id,
                mode=mode,
                freq=freq,
                hard=hard,
                callback=callback,
            )
            sleep_ms(9)
            timer.deinit()
