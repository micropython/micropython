import bench


def test(num):
    i = 0
    d = set()  # object with largest rom-frozen ordered locals_dict
    s = "__contains__"  # last element in that dict for longest lookup
    while i < 20000000 and hasattr(d, s):
        i += 1


bench.run(test)
