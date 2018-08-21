import board
import digitalio
import time

led = digitalio.DigitalInOut(board.LED2)
led.direction = digitalio.Direction.OUTPUT

while True:
    led.value = True
    time.sleep(0.5)
    led.value = False
    time.sleep(0.5)
