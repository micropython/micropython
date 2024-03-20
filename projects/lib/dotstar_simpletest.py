import time
import random
import micropython_dotstar as dotstar
from machine import Pin, SPI

# On-board DotStar for the TinyPICO
spi = SPI(sck=Pin(8), mosi=Pin(10), miso=Pin(9)) # Configure SPI - note: miso is unused
dots = dotstar.DotStar(spi, 30, brightness=0.5)

# Using a DotStar Digital LED Strip with 30 LEDs connected to SPI
# dots = dotstar.DotStar(spi=SPI(sck=Pin(x), mosi=Pin(y)), 30, brightness=0.2)

# HELPERS
# a random color 0 -> 224
def random_color():
    return random.randrange(0, 7) * 32


# MAIN LOOP
n_dots = len(dots)
while True:
    # Fill each dot with a random color
    for dot in range(n_dots):
        dots[dot] = (random_color(), random_color(), random_color())

    time.sleep(.25)
