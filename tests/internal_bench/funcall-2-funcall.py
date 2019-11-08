# Function call overhead test
# Perform the same trivial operation as global function call
import bench

def f(x):
    return x + 1

def test(num):
    for i in iter(range(num)):
        a = f(i)

bench.run(test)
