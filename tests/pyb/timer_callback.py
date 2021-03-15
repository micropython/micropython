# check callback feature of the timer class

import pyb
from pyb import Timer

# callback function that disables the callback when called
def cb1(t):
    print("cb1")
    t.callback(None)


# callback function that disables the timer when called
def cb2(t):
    print("cb2")
    t.deinit()


# callback where cb4 closes over cb3.y
def cb3(x):
    y = x

    def cb4(t):
        print("cb4", y)
        t.callback(None)

    return cb4


# create a timer with a callback, using callback(None) to stop
tim = Timer(1, freq=100, callback=cb1)
pyb.delay(5)
print("before cb1")
pyb.delay(15)

# create a timer with a callback, using deinit to stop
tim = Timer(2, freq=100, callback=cb2)
pyb.delay(5)
print("before cb2")
pyb.delay(15)

# create a timer, then set the freq, then set the callback
tim = Timer(4)
tim.init(freq=100)
tim.callback(cb1)
pyb.delay(5)
print("before cb1")
pyb.delay(15)

# test callback with a closure
tim.init(freq=100)
tim.callback(cb3(3))
pyb.delay(5)
print("before cb4")
pyb.delay(15)
