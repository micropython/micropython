import bench


def test(num):
    for i in range(num // 40):

        class X:
            def __getattr__(self, name):
                pass


bench.run(test)
