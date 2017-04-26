# test micropython.schedule() function

import micropython

try:
    micropython.schedule
except AttributeError:
    print('SKIP')
    import sys
    sys.exit()

# Basic test of scheduling a function.

def callback(arg):
    global done
    print(arg)
    done = True

done = False
micropython.schedule(callback, 1)
while not done:
    pass

# Test that callbacks can be scheduled from within a callback, but
# that they don't execute until the outer callback is finished.

def callback_inner(arg):
    global done
    print('inner')
    done += 1

def callback_outer(arg):
    global done
    micropython.schedule(callback_inner, 0)
    # need a loop so that the VM can check for pending events
    for i in range(2):
        pass
    print('outer')
    done += 1

done = 0
micropython.schedule(callback_outer, 0)
while done != 2:
    pass

# Test that scheduling too many callbacks leads to an exception.  To do this we
# must schedule from within a callback to guarantee that the scheduler is locked.

def callback(arg):
    global done
    try:
        for i in range(100):
            micropython.schedule(lambda x:x, None)
    except RuntimeError:
        print('RuntimeError')
    done = True

done = False
micropython.schedule(callback, None)
while not done:
    pass
