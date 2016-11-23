"""
LED fade/flash example using asyncio.

This example uses continually fades a LED on and off, and switches to a
quicker flash for a small period when a button is pressed.

The LED and BUTTON objects are imported from the board/hardware config module.
You need to copy the config file for your board to a suitable location.

Tested on the Olimex E407 board, which has one LED and one button.
See examples/hwapi/hwconfig_OLIMEX_E407.py for an example config module

This example was extended from LED fade example using asyncio
https://github.com/micropython/micropython/commit/99e5badeb1e2e6aeffd3b3d56902d4257e168326
"""

import uasyncio
from hwconfig import LED, BUTTON

#=============================================================================

async def pwm_cycle(led, duty, cycles):
    """A task to flash or fade the LED depending global count variable."""

    duty_off = 20 - duty
    for i in range(cycles):
        if duty:
            led.value(1)
            await uasyncio.sleep_ms(duty)
        if duty_off:
            led.value(0)
            await uasyncio.sleep_ms(duty_off)

#=============================================================================

async def fade_led(led):
    """A task to fade the LED on and off."""

    cycles = 1
    # Fade off to on
    for duty in range(1, 21):
        await pwm_cycle(led, duty, cycles)
    # Fade on to off
    for duty in range(20, 0, -1):
        await pwm_cycle(led, duty, cycles)

#=============================================================================

async def flash_led(led, delay):
    """A task to flash or fade the LED."""

    # led on
    led.value(1)
    await uasyncio.sleep_ms(delay)
    # led off
    led.value(0)
    await uasyncio.sleep_ms(delay)

#=============================================================================

async def update_led(led):
    """A task to flash or fade the LED depending global count variable."""

    global flash_count
    flash_count = 0

    while True:
        if flash_count > 0:
            #print("DEBUG: flashing")
            await flash_led(led, 100)
            flash_count -= 1
        else:
            #print("DEBUG: pulsing")
            await fade_led(led)

#=============================================================================

async def check_button(button):
    """A task to continually check if button is pressed and set a global count variable."""

    global flash_count

    while True:
        await uasyncio.sleep_ms(100)
        if button.value():
            flash_count = 10

#=============================================================================

loop = uasyncio.get_event_loop()
# Create tasks to check the button.
loop.create_task(check_button(BUTTON))
# Create tasks to update the LED.
loop.create_task(update_led(LED))
# Start the tasks.
loop.run_forever()

