import bench


def test(num):
    for i in range(num // 40):

        class X:
            def __new__(cls):
                pass


bench.run(test)
