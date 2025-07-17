# test _thread.get_ident() function
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread


# Initialise variables (also preallocate their spot in the globals dict so the
# globals dict is not resized while threads are running).
tid = None
tid_main = None
new_tid = None
finished = False


def thread_entry():
    global tid
    tid = _thread.get_ident()
    print("thread", type(tid) == int, tid != 0, tid != tid_main)
    global finished
    finished = True


tid_main = _thread.get_ident()
print("main", type(tid_main) == int, tid_main != 0)

new_tid = _thread.start_new_thread(thread_entry, ())

while not finished:
    time.sleep(0)

print("done", type(new_tid) == int, new_tid == tid)
