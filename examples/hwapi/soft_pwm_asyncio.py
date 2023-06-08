# See original soft_pwm.py for detailed comments.
import asyncio
from hwconfig import LED


async def pwm_cycle(led, duty, cycles):
    duty_off = 20 - duty
    for i in range(cycles):
        if duty:
            led.value(1)
            await asyncio.sleep_ms(duty)
        if duty_off:
            led.value(0)
            await asyncio.sleep_ms(duty_off)


async def fade_in_out(LED):
    while True:
        # Fade in
        for i in range(1, 21):
            await pwm_cycle(LED, i, 2)
        # Fade out
        for i in range(20, 0, -1):
            await pwm_cycle(LED, i, 2)


loop = asyncio.get_event_loop()
loop.run_until_complete(fade_in_out(LED))
