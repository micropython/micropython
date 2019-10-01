import bench

def test(num):
    while num > 0:
        num -= 1

bench.run(test)
