# Test the limit of the number of arguments to a function call.
# This currently tests the case of *args after many positional args.


def f(*args):
    return len(args)


def test(n):
    pos_args = ",".join(str(i) for i in range(n))
    s = "f({}, *(100, 101), 102, 103)".format(pos_args)
    try:
        return eval(s)
    except SyntaxError:
        return "SyntaxError"


# If the port has at least 32-bits then this test should pass.
print(test(29))

# This test should fail on all ports (overflows a small int).
print(test(70))

# Check that there is a correct transition to the limit of too many args before *args.
reached_limit = False
for i in range(30, 70):
    result = test(i)
    if reached_limit:
        if result != "SyntaxError":
            print("FAIL")
    else:
        if result == "SyntaxError":
            reached_limit = True
        else:
            if result != i + 4:
                print("FAIL")
