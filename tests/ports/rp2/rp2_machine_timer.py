from machine import Timer
from time import sleep_ms

# Test the rp2-specific adjustable tick_hz and hard/soft IRQ handlers
# for both one-shot and periodic timers.

modes = {Timer.ONE_SHOT: "one-shot", Timer.PERIODIC: "periodic"}
kinds = {False: "soft", True: "hard"}

for mode in modes:
    for hard in kinds:
        for hz in 1000, 2000:
            timer = machine.Timer(
                period=2,
                mode=mode,
                hard=hard,
                tick_hz=hz,
                callback=lambda t: print("callback", modes[mode], kinds[hard], hz),
            )
            sleep_ms(5)
            timer.deinit()
