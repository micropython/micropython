import sys

try:
    from machine import Timer
    from time import sleep_ms
except:
    print("SKIP")
    raise SystemExit

if sys.platform in ("esp8266", "pyboard", "rp2", "stm32", "zephyr") or sys.platform.startswith(
    "nrf"
):
    # Hardware timers are not supported on the esp8266, rp2, stm32, and zephyr ports.
    # The nrf port supports hardware timers, however its interface is incompatible:
    # It doesn't support the .init() method, doesn't support the freq argument, the
    # period argument unit is us instead ms, ONE_SHOT constant is named ONESHOT.
    print("SKIP")
    raise SystemExit

# create and deinit
t = Timer(0)
t.init(freq=1)
t.deinit()

# deinit again
t.deinit()

# init a large number of times
for _ in range(256):
    t.init(freq=1)
t.deinit()


# run one-shot and periodic timers with different frequencies and periods
def callback(_):
    print("callback", mode[1], freq_period, end=" ")
    try:
        allocate = bytearray(1)
        print("unlocked")
    except MemoryError:
        print("locked")


for mode in [(Timer.ONE_SHOT, "one-shot"), (Timer.PERIODIC, "periodic")]:
    for freq_period in ({"freq": 50}, {"freq": 25}, {"period": 20}, {"period": 40}):
        t = Timer(0)
        t.init(
            mode=mode[0],
            callback=callback,
            hard=False,
            **freq_period,
        )
        print("inited")
        sleep_ms(10)
        print("after 10ms")
        sleep_ms(20)
        print("after 30ms")
        sleep_ms(40)
        print("after 70ms")
        sleep_ms(20)
        print("deiniting")
        t.deinit()
