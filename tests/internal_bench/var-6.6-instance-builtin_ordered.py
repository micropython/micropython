import bench


def test(num):
    i = 0
    o = set()  # object with largest rom-frozen ordered locals_dict
    n = "__contains__"  # last element in that dict for longest lookup
    while i < num:
        i += hasattr(o, n)  # True, converts to 1


bench.run(test)
