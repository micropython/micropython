import bench


class Class:
    pass


def test(num):
    instance = Class()
    for i in range(num // 40):

        class X:
            a = instance
            b = instance
            c = instance
            d = instance
            x = instance


bench.run(test)
