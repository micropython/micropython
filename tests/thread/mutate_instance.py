# test concurrent mutating access to a shared user instance
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread


# the shared user class and instance
class User:
    def __init__(self):
        self.a = "A"
        self.b = "B"
        self.c = "C"


user = User()


# main thread function
def th(n, lo, hi):
    for repeat in range(n):
        for i in range(lo, hi):
            setattr(user, "attr_%u" % i, repeat + i)
            assert getattr(user, "attr_%u" % i) == repeat + i

    with lock:
        global n_finished
        n_finished += 1


lock = _thread.allocate_lock()
n_repeat = 30
n_range = 50  # 300 for stressful test (uses more heap)
n_thread = 4
n_finished = 0

# spawn threads
for i in range(n_thread):
    _thread.start_new_thread(th, (n_repeat, i * n_range, (i + 1) * n_range))

# busy wait for threads to finish
while n_finished < n_thread:
    time.sleep(0)

# check user instance has correct contents
print(user.a, user.b, user.c)
for i in range(n_thread * n_range):
    assert getattr(user, "attr_%u" % i) == n_repeat - 1 + i
