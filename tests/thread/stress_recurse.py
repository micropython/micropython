# test hitting the function recursion limit within a thread
#
# SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
#
# SPDX-License-Identifier: MIT

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
    pass
print("done")
