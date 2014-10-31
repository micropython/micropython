# check that we can do certain things without allocating heap memory

import gc

def f(a):
    print(a)

def g(a, b=2):
    print(a, b)

global_var = 1

def h():
    global global_var
    global_var = 2      # set an existing global variable
    for i in range(2):  # for loop
        f(i)            # function call
        f(i * 2 + 1)    # binary operation with small ints
        f(a=i)          # keyword arguments
        g(i)            # default arg (second one)
        g(i, i)         # 2 args

# call h with heap allocation disabled and all memory used up
gc.disable()
try:
    while True:
        'a'.lower # allocates 1 cell for boundmeth
except MemoryError:
    pass
h()
gc.enable()
