# test constant optimisation, with consts that are floats

from micropython import const

# check we can make consts from floats
F1 = const(2.5)
F2 = const(-0.3)
print(type(F1), F1)
print(type(F2), F2)

# check arithmetic with floats
F3 = const(F1 + F2)
F4 = const(F1**2)
print(F3, F4)

# check int operations with float results
F5 = const(1 / 2)
F6 = const(2**-2)
print(F5, F6)

# note: we also test float expression folding when
#       we're compiling test cases in tests/float, as
#       many expressions are resolved at compile time.
