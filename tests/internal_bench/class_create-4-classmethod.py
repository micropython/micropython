import bench


def test(num):
    for i in range(num // 40):

        class X:
            @classmethod
            def x(cls):
                pass


bench.run(test)
