import bench


def test(num):
    class B:
        @classmethod
        def __init_subclass__(cls):
            pass

    for i in range(num // 40):

        class X(B):
            pass


bench.run(test)
