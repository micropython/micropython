import sys

try:
    from machine import Timer
    from time import sleep_ms
except:
    print("SKIP")
    raise SystemExit

if sys.platform == "esp32" or sys.platform.startswith("nrf"):
    # Hard IRQ's are not supported on the esp32 port.
    # Virtual timers are not supported on the nrf port.
    print("SKIP")
    raise SystemExit

# create and deinit
t = Timer(-1)
t.init(freq=1)
t.deinit()

# deinit again
t.deinit()

# init a large number of times
for _ in range(256):
    t.init(freq=1)
t.deinit()

# create 2 and deinit
t = Timer(-1)
t.init(freq=1)
t2 = Timer(-1)
t2.init(freq=1)
t.deinit()
t2.deinit()

# create 2 and deinit in different order
t = Timer(-1)
t.init(freq=1)
t2 = Timer(-1)
t.init(freq=1)
t2.deinit()
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
        t = Timer(-1)
        t.init(
            mode=mode[0],
            callback=callback,
            hard=True,
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
