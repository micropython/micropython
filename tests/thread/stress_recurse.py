# test hitting the function recursion limit within a thread
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread


def foo():
    foo()


def thread_entry():
    try:
        foo()
    except RuntimeError:
        print("RuntimeError")
    global finished
    finished = True


finished = False

_thread.start_new_thread(thread_entry, ())

# busy wait for thread to finish
while not finished:
    time.sleep(0)
print("done")
