# Test freezing constants.

x0 = (1,)
x1 = (1, 2)
x2 = (1, 1 << 100)
x3 = (None, False, True, ...)
x4 = ("str", b"bytes")
x5 = ((),)
x6 = ((1,),)
x7 = ((1, 2),)
x8 = (1, "str", (), ("nested", 2, ((False, True), None, ...)))
