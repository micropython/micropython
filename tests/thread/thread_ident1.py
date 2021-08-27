# test _thread.get_ident() function
#
# SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
#
# SPDX-License-Identifier: MIT

import _thread


def thread_entry():
    tid = _thread.get_ident()
    print("thread", type(tid) == int, tid != 0, tid != tid_main)
    global finished
    finished = True


tid_main = _thread.get_ident()
print("main", type(tid_main) == int, tid_main != 0)

finished = False
_thread.start_new_thread(thread_entry, ())

while not finished:
    pass
print("done")
