from machine import Pin
import time

# Define the pin number (adjust the pin name according to your board's documentation)
led_pin = 'IO_L08_0'

# Initialize the pin as an output
led = Pin(led_pin, Pin.OUT)

while True:
    led.value(0)   # Turn the LED off
    time.sleep(1) # Wait for 1 seconds
    led.value(1)   # Turn the LED on
    time.sleep(1) # Wait for another 1 seconds
