# check that we can do certain things without allocating heap memory

import micropython

# Check for stackless build, which can't call functions without
# allocating a frame on heap.
try:

    def stackless():
        pass

    micropython.heap_lock()
    stackless()
    micropython.heap_unlock()
except RuntimeError:
    print("SKIP")
    raise SystemExit


def f1(a):
    print(a)


def f2(a, b=2):
    print(a, b)


def f3(a, b, c, d):
    x1 = x2 = a
    x3 = x4 = b
    x5 = x6 = c
    x7 = x8 = d
    print(x1, x3, x5, x7, x2 + x4 + x6 + x8)


global_var = 1


def test():
    global global_var, global_exc
    global_var = 2  # set an existing global variable
    for i in range(2):  # for loop
        f1(i)  # function call
        f1(i * 2 + 1)  # binary operation with small ints
        f1(a=i)  # keyword arguments
        f2(i)  # default arg (second one)
        f2(i, i)  # 2 args
    f3(1, 2, 3, 4)  # function with lots of local state


# call test() with heap allocation disabled
micropython.heap_lock()
test()
micropython.heap_unlock()
