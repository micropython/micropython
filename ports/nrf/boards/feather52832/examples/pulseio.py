import time
from board import *
from pulseio import *

# Setup BLUE and RED LEDs as PWM output (default frequency is 500 Hz)
ledb = PWMOut(LED2)
ledr = PWMOut(LED1)

# Set the BLUE LED to have a duty cycle of 5000 (out of 65535, so ~7.5%)
ledb.duty_cycle = 5000

# Setup pin A0 as a standard PWM out @ 50% to test on the oscilloscope.
# You should see a 50% duty cycle waveform at ~500Hz on the scope when you
# connect a probe to pin A0
a0 = PWMOut(A0)
a0.duty_cycle = int(65535/2)

# Constantly pulse the RED LED
while True:
    for i in range(100):
        ledr.duty_cycle = int(i / 100 * 65535)
        time.sleep(0.01)
    for i in range(100, -1, -1):
        ledr.duty_cycle = int(i / 100 * 65535)
        time.sleep(0.01)
