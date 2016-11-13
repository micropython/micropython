# Like soft_pwm_uasyncio.py, but fading 2 LEDs with different phase.
# Also see original soft_pwm.py.
import uasyncio
from hwconfig import LED, LED2


async def pwm_cycle(led, duty, cycles):
    duty_off = 20 - duty
    for i in range(cycles):
        if duty:
            led.value(1)
            await uasyncio.sleep_ms(duty)
        if duty_off:
            led.value(0)
            await uasyncio.sleep_ms(duty_off)


async def fade_in_out(LED):
    while True:
        # Fade in
        for i in range(1, 21):
            await pwm_cycle(LED, i, 2)
        # Fade out
        for i in range(20, 0, -1):
            await pwm_cycle(LED, i, 2)


loop = uasyncio.get_event_loop()
loop.create_task(fade_in_out(LED))
loop.call_later_ms_(800, fade_in_out(LED2))
loop.run_forever()
