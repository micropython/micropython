# test using lock to coordinate access to global mutable objects
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread


def fac(n):
    x = 1
    for i in range(1, n + 1):
        x *= i
    return x


def thread_entry():
    while True:
        with jobs_lock:
            try:
                f, arg = jobs.pop(0)
            except IndexError:
                return
        ans = f(arg)
        with output_lock:
            output.append((arg, ans))


# create a list of jobs
jobs = [(fac, i) for i in range(20, 80)]
jobs_lock = _thread.allocate_lock()
n_jobs = len(jobs)

# create a list to store the results
output = []
output_lock = _thread.allocate_lock()

# spawn threads to do the jobs
for i in range(4):
    try:
        _thread.start_new_thread(thread_entry, ())
    except OSError:
        # System cannot create a new thead, so stop trying to create them.
        break

# wait for the jobs to complete
while True:
    with jobs_lock:
        if len(output) == n_jobs:
            break
    time.sleep(0)

# sort and print the results
output.sort(key=lambda x: x[0])
for arg, ans in output:
    print(arg, ans)
