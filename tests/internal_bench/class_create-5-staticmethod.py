import bench


def test(num):
    for i in range(num // 40):

        class X:
            @staticmethod
            def x():
                pass


bench.run(test)
