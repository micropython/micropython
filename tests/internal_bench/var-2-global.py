import bench

ITERS = 20000000

def test(num):
    i = 0
    while i < ITERS:
        i += 1

bench.run(test)
