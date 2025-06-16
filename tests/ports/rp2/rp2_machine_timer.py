from machine import Timer
from time import sleep_ms

# Test the rp2-specific adjustable tick_hz and hard/soft IRQ handlers
# for both one-shot and periodic timers.

modes = {Timer.ONE_SHOT: "one-shot", Timer.PERIODIC: "periodic"}
kinds = {False: "soft", True: "hard"}

for mode in modes:
    for hard in kinds:
        for period in 2, 4:
            timer = Timer(
                mode=mode,
                period=period,
                hard=hard,
                callback=lambda t: print("callback", modes[mode], kinds[hard], period),
            )
            sleep_ms(9)
            timer.deinit()
