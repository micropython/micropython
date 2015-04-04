# test errors from bad function calls

def test_exc(code, exc):
    try:
        exec(code)
        print("no exception")
    except exc:
        print("right exception")
    except:
        print("wrong exception")

# function doesn't take keyword args
test_exc("[].append(x=1)", TypeError)

# function with variable number of positional args given too few
test_exc("round()", TypeError)

# function with variable number of positional args given too many
test_exc("round(1, 2, 3)", TypeError)

# function with fixed number of positional args given wrong number
test_exc("[].append(1, 2)", TypeError)

# function with keyword args given extra positional args
test_exc("[].sort(1)", TypeError)

# function with keyword args given extra keyword args
test_exc("[].sort(noexist=1)", TypeError)

# function with keyword args not given a specific keyword arg
test_exc("enumerate()", TypeError)

# kw given for positional, but a different positional is missing
test_exc("def f(x, y): pass\nf(x=1)", TypeError)
