try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit


def test(a):
    print(a)
    a.append(1.2)
    print(len(a), "%.3f" % a[0])
    a.append(1)
    a.append(False)
    print(len(a), "%.3f %.3f" % (a[1], a[2]))
    a[-1] = 3.45
    print("%.3f" % a[-1])


test(array("f"))
test(array("d"))

# hand-crafted floats, including non-standard nan
for float_hex in (0x3DCCCCCC, 0x7F800024, 0x7FC00004):
    f = array("f", bytes(array("I", [float_hex])))[0]
    if type(f) is float:
        print("{:.4e}".format(f))
    else:
        print(f)
