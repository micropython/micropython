# Function call overhead test
# Perform the same trivial operation as calling function, cached in a
# local variable. This is commonly known optimization for overly dynamic
# languages (the idea is to cut on symbolic look up overhead, as local
# variables are accessed by offset, not by name)
import bench

def f(x):
    return x + 1

def test(num):
    f_ = f
    for i in iter(range(num)):
        a = f_(i)

bench.run(test)
