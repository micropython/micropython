# test errors from bad operations (unary, binary, etc)

def test_exc(code, exc):
    try:
        exec(code)
        print("no exception")
    except exc:
        print("right exception")
    except:
        print("wrong exception")

# object with buffer protocol needed on rhs
try:
    (1 << 70) in 1
except TypeError:
    print('TypeError')
