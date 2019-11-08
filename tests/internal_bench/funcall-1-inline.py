# Function call overhead test
# Establish a baseline for performing a trivial operation inline
import bench

def test(num):
    for i in iter(range(num)):
        a = i + 1

bench.run(test)
