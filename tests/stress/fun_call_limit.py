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
print(test(28))

# This test should fail on all ports (overflows a small int).
print(test(70))

# 28 is the biggest number that will pass on a 32-bit port using object
# representation B, which has 1<<28 still fitting in a positive small int.
reached_limit = False
any_test_succeeded = False
for i in range(28, 70):
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
            any_test_succeeded = True
assert any_test_succeeded  # At least one iteration must have passed
