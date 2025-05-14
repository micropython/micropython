# test _thread.exit() function
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread


def thread_entry():
    _thread.exit()


for i in range(2):
    while True:
        try:
            _thread.start_new_thread(thread_entry, ())
            break
        except OSError:
            pass

# wait for threads to finish
time.sleep(1)
print("done")
