# log the accelerometer values to a file, 1 per second

f = open('motion.dat', 'w')                 # open the file for writing

for i in range(60):                         # loop 60 times
    time = pyb.time()                       # get the current time
    accel = pyb.accel()                     # get the accelerometer data

    # write time and x,y,z values to the file
    f.write('{} {} {} {}\n'.format(time, accel[0], accel[1], accel[2]))

    pyb.delay(1000)                         # wait 1000 ms = 1 second

f.close()                                   # close the file
