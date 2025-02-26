import bench


def test(num):
    for i in range(num // 40):

        class X:
            @property
            def x(self):
                pass


bench.run(test)
