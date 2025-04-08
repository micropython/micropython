from inkplate6 import Inkplate
from image import *

display = Inkplate(Inkplate.INKPLATE_1BIT)

if __name__ == "__main__":
    display.begin()
    display.clearDisplay()
    display.display()

    battery = str(display.readBattery())

    display.setTextSize(2)
    display.printText(100, 100, "batt: " + battery + "V")
    display.display()

    
    temperature = str(display.readTemperature())

    display.setTextSize(2)
    display.printText(100, 150, "TEMP: " + temperature + "C")
    display.display()