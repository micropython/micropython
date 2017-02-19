# test errors from bad operations (unary, binary, etc)

def test_exc(code, exc):
    try:
        exec(code)
        print("no exception")
    except exc:
        print("right exception")
    except:
        print("wrong exception")

# unsupported unary operators
test_exc("~None", TypeError)
test_exc("~''", TypeError)
test_exc("~[]", TypeError)
test_exc("~bytearray()", TypeError)

# unsupported binary operators
test_exc("False in True", TypeError)
test_exc("1 * {}", TypeError)
test_exc("1 in 1", TypeError)
test_exc("bytearray() // 2", TypeError)

# object with buffer protocol needed on rhs
test_exc("bytearray(1) + 1", TypeError)
test_exc("(1 << 70) in 1", TypeError)

# unsupported subscription
test_exc("1[0]", TypeError)
test_exc("1[0] = 1", TypeError)
test_exc("''['']", TypeError)
test_exc("'a'[0] = 1", TypeError)
test_exc("del 1[0]", TypeError)

# not callable
test_exc("1()", TypeError)

# not an iterator
test_exc("next(1)", TypeError)

# must be an exception type
test_exc("raise 1", TypeError)

# no such name in import
test_exc("from sys import youcannotimportmebecauseidontexist", ImportError)
