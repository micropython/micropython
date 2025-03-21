# Include needed libraries

from inkplate6 import Inkplate
from image import *
import time


# Initialize inkplate display
display = Inkplate(Inkplate.INKPLATE_2BIT)


# Main function, you can make infinite while loop inside this to run code indefinitely
if __name__ == "__main__":
    # Must be called before using, line in Arduino
    display.begin()
    display.clearDisplay()
    display.display()

    # Draw palet of posible colors
    #use color values 0, 1, 2, 3
    display.writeFillRect(0, 0, 25, 600, 3)
    display.writeFillRect(25, 0, 25, 600, 2)
    display.writeFillRect(50, 0, 25, 600, 1)
    display.writeFillRect(75, 0, 25, 600, 0)

    display.display()
    time.sleep(3)

    # Draws image from bytearray
    display.setRotation(0)
    display.drawBitmap(120, 200, image, 576, 100)
    display.display()
    time.sleep(10)
