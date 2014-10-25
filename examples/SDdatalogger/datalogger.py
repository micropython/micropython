# datalogger.py
# Logs the data from the acceleromter to a file on the SD-card

import pyb

# creating objects
accel = pyb.Accel()
blue = pyb.LED(4)
switch = pyb.Switch()

# loop
while True:

    # wait for interrupt
    # this reduces power consumption while waiting for switch press
    pyb.wfi()

    # start if switch is pressed
    if switch():
        pyb.delay(200)                      # delay avoids detection of multiple presses
        blue.on()                           # blue LED indicates file open
        log = open('/sd/log.csv', 'w')       # open file on SD (SD: '/sd/', flash: '/flash/)

        # until switch is pressed again
        while not switch():
            t = pyb.millis()                            # get time
            x, y, z = accel.filtered_xyz()              # get acceleration data
            log.write('{},{},{},{}\n'.format(t,x,y,z))  # write data to file

        # end after switch is pressed again
        log.close()                         # close file
        blue.off()                          # blue LED indicates file closed
        pyb.delay(200)                      # delay avoids detection of multiple presses
