from inkplate6 import Inkplate
from image import *

display = Inkplate(Inkplate.INKPLATE_1BIT)

circle_x = 400
circle_y = 300
circle_r = 40

#main function used by micropython
if __name__ == "__main__":
    display.begin()
    display.clearDisplay()
    display.display()

    # function to show text at the top of the screen
    # need to be called every time we clear display
    def topText():
        display.setTextSize(2)
        display.printText(100, 10, "TOUCHPADS EXAMPLE! 1, 3 TO MOVE CIRCLE, 2 TO RESET")

    topText()

    # Touchpads definitions
    touch1, touch2, touch3 = display.TOUCH1, display.TOUCH2, display.TOUCH3
    
    #draw initial circle for touchpad demonstration
    display.drawCircle(circle_x, circle_y, circle_r, display.BLACK)
    display.display()

    #Main loop that will run forever or until battery is dead
    while True:
        #check if touchpad is pressed
        if touch1():
            circle_x -= 40
            display.clearDisplay()
            topText()
            display.drawCircle(circle_x, circle_y, circle_r, display.BLACK)
            display.partialUpdate()

        if touch3():
            circle_x += 40
            display.clearDisplay()
            topText()
            display.drawCircle(circle_x, circle_y, circle_r, display.BLACK)
            display.partialUpdate()

        if touch2():
            circle_x = 400
            circle_y = 300
            circle_r = 40

            display.clearDisplay()
            topText()
            display.drawCircle(circle_x, circle_y, circle_r, display.BLACK)
            display.partialUpdate()

