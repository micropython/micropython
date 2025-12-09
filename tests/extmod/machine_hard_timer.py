import sys

try:
    from machine import Timer
    from time import sleep_ms
except:
    print("SKIP")
    raise SystemExit

if sys.platform == "esp8266":
    timer = Timer(0)
else:
    # Hardware timers are not implemented.
    print("SKIP")
    raise SystemExit

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
            timer.init(
                mode=mode[0],
                freq=freq,
                hard=kind[0],
                callback=callback,
            )
            sleep_ms(90)
            timer.deinit()
