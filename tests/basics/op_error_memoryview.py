# test errors from bad operations (unary, binary, etc)
try:
    memoryview
except:
    print("SKIP")
    raise SystemExit

def test_exc(code, exc):
    try:
        exec(code)
        print("no exception")
    except exc:
        print("right exception")
    except:
        print("wrong exception")

# unsupported binary operators
test_exc("m = memoryview(bytearray())\nm += bytearray()", TypeError)
