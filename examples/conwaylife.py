#import essential libraries
import lcd
import pyb

# do 1 iteration of Conway's Game of Life
def conway_step():
    for x in range(128):        # loop over x coordinates
        for y in range(32):     # loop over y coordinates
            # count number of neigbours
            num_neighbours = (lcd.get(x - 1, y - 1) +
                lcd.get(x, y - 1) +
                lcd.get(x + 1, y - 1) +
                lcd.get(x - 1, y) +
                lcd.get(x + 1, y) +
                lcd.get(x + 1, y + 1) +
                lcd.get(x, y + 1) +
                lcd.get(x - 1, y + 1))

            # check if the centre cell is alive or not
            self = lcd.get(x, y)

            # apply the rules of life
            if self and not (2 <= num_neighbours <= 3):
                lcd.reset(x, y) # not enough, or too many neighbours: cell dies
            elif not self and num_neighbours == 3:
                lcd.set(x, y)   # exactly 3 neigbours around an empty cell: cell is born

# randomise the start
def conway_rand():
    lcd.clear()                 # clear the LCD
    for x in range(128):        # loop over x coordinates
        for y in range(32):     # loop over y coordinates
            if pyb.rand() & 1:  # get a 1-bit random number
                lcd.set(x, y)   # set the pixel randomly

# loop for a certain number of frames, doing iterations of Conway's Game of Life
def conway_go(num_frames):
    for i in range(num_frames):
        conway_step()           # do 1 iteration
        lcd.show()              # update the LCD
        pyb.delay(300)

# PC testing
lcd = lcd.LCD(128, 32)
conway_rand()
conway_go(1000)
