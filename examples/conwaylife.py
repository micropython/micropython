# import essential libraries
import pyb

lcd = pyb.LCD("x")
lcd.light(1)

# do 1 iteration of Conway's Game of Life
def conway_step():
    for x in range(128):  # loop over x coordinates
        for y in range(32):  # loop over y coordinates
            # count number of neighbours
            num_neighbours = (
                lcd.get(x - 1, y - 1)
                + lcd.get(x, y - 1)
                + lcd.get(x + 1, y - 1)
                + lcd.get(x - 1, y)
                + lcd.get(x + 1, y)
                + lcd.get(x + 1, y + 1)
                + lcd.get(x, y + 1)
                + lcd.get(x - 1, y + 1)
            )

            # check if the centre cell is alive or not
            self = lcd.get(x, y)

            # apply the rules of life
            if self and not (2 <= num_neighbours <= 3):
                lcd.pixel(x, y, 0)  # not enough, or too many neighbours: cell dies
            elif not self and num_neighbours == 3:
                lcd.pixel(x, y, 1)  # exactly 3 neighbours around an empty cell: cell is born


# randomise the start
def conway_rand():
    lcd.fill(0)  # clear the LCD
    for x in range(128):  # loop over x coordinates
        for y in range(32):  # loop over y coordinates
            lcd.pixel(x, y, pyb.rng() & 1)  # set the pixel randomly


# loop for a certain number of frames, doing iterations of Conway's Game of Life
def conway_go(num_frames):
    for i in range(num_frames):
        conway_step()  # do 1 iteration
        lcd.show()  # update the LCD
        pyb.delay(50)


# testing
conway_rand()
conway_go(100)
