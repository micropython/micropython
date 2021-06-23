# test raising SystemExit to finish a thread
#
# SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
#
# SPDX-License-Identifier: MIT

try:
    import utime as time
except ImportError:
    import time
import _thread


def thread_entry():
    raise SystemExit


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
