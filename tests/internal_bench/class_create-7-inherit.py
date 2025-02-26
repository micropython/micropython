import bench


def test(num):
    class B:
        pass

    for i in range(num // 40):

        class X(B):
            pass


bench.run(test)
