import alarm
import board
import time
import digitalio
import neopixel

## WAKING PINS - uncomment appropriate pin per microcontroller
wake_pin = board.X1  # STM32F4 Pyboard
# wake_pin = board.GP0                    # RP2040 Pico
# wake_pin = board.A4                     # NRF52840 Feather
# wake_pin = board.IO5                    # ESP32-S2 Saola

## LED - use on RP2040 Pico, STM32F4 Pyboard
## PinAlarms blink 1x, TimeAlarms 2x, Startup 3x
led_pin = board.LED
led = digitalio.DigitalInOut(led_pin)
led.direction = digitalio.Direction.OUTPUT


def blink(num_blinks):
    for i in range(num_blinks):
        led.value = True
        time.sleep(0.2)
        led.value = False
        time.sleep(0.2)


def show_timealarm():
    blink(2)


def show_pinalarm():
    blink(1)


def show_noalarm():
    blink(3)


## Comment out above if using Neopixel

## NEOPIXEL - use on Circuitplayground Bluefruit, ESP32-S2 Saola
## TimeAlarms are red, PinAlarms are blue, Default is white
# np = neopixel.NeoPixel(board.NEOPIXEL, 1)
# def show_timealarm():
#     np[0] = (50, 0, 0)
#     time.sleep(1)
#     np[0] = (0, 0, 0)
# def show_pinalarm():
#     np[0] = (0, 0, 50)
#     time.sleep(1)
#     np[0] = (0, 0, 0)
# def show_noalarm():
#     np[0] = (50, 50, 50)
#     time.sleep(1)
#     np[0] = (0, 0, 0)
## Comment out above if using LED

## PinAlarm only needs to be set once
pin_alarm = alarm.pin.PinAlarm(pin=wake_pin, value=False, edge=True, pull=True)  # STM32, RP2040
# pin_alarm = alarm.pin.PinAlarm(pin=wake_pin, value=False, edge=False, pull=True)  # NRF, ESP32-S2, RP2040

while True:
    ## TimeAlarms must be reset each time you sleep, since they use monotonic time
    time_alarm = alarm.time.TimeAlarm(monotonic_time=time.monotonic() + 10)
    ret_alarm = alarm.light_sleep_until_alarms(pin_alarm, time_alarm)

    print("Returned alarm vs global alarm:")  # These should be the same
    print(ret_alarm)
    print(alarm.wake_alarm)

    if isinstance(ret_alarm, alarm.time.TimeAlarm):
        show_timealarm()
    elif isinstance(ret_alarm, alarm.pin.PinAlarm):
        show_pinalarm()
    else:
        show_noalarm()
