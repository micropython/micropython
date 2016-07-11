import bench

def test(num):
    for i in iter(range(num // 1000)):
        bytes(10000)

bench.run(test)
