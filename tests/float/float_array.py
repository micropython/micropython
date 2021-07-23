try:
    from uarray import array
except ImportError:
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

print("{:.4f}".format(array("f", bytes(array("I", [0x3DCCCCCC])))[0]))
