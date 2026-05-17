import sys

try:
    from machine import Timer
    from time import sleep_ms
except:
    print("SKIP")
    raise SystemExit

if sys.platform in ("esp32", "esp8266", "nrf"):
    # Software timers aren't implemented on the esp32 and esp8266 ports.
    # The nrf port doesn't support selection of hard and soft callbacks,
    # and only allows Timer(period=N), not Timer(freq=N).
    print("SKIP")
    raise SystemExit
else:
    timer_id = -1

# Test both hard and soft IRQ handlers and both one-shot and periodic
# timers. We adjust period in tests/extmod/machine_soft_timer.py, so try
# adjusting freq here instead. The heap should be locked in hard callbacks
# and unlocked in soft callbacks.


def callback(t):
    print("callback", mode[1], kind[1], freq, end=" ")
    try:
        allocate = bytearray(1)
        print("unlocked")
    except MemoryError:
        print("locked")


modes = [(Timer.ONE_SHOT, "one-shot"), (Timer.PERIODIC, "periodic")]
kinds = [(False, "soft"), (True, "hard")]

for mode in modes:
    for kind in kinds:
        for freq in 50, 25:
            timer = Timer(
                timer_id,
                mode=mode[0],
                freq=freq,
                hard=kind[0],
                callback=callback,
            )
            sleep_ms(90)
            timer.deinit()
