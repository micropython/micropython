# tests meminfo functions in micropython module

import micropython

# these functions are not always available
if not hasattr(micropython, 'mem_total'):
    print('SKIP')
else:
    t = micropython.mem_total()
    c = micropython.mem_current()
    p = micropython.mem_peak()

    l = list(range(10000))

    print(micropython.mem_total() > t)
    print(micropython.mem_current() > c)
    print(micropython.mem_peak() > p)
