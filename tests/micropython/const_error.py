# make sure syntax error works correctly for bad const definition

from micropython import const


def test_syntax(code):
    try:
        exec(code)
    except SyntaxError:
        print("SyntaxError")


# argument not a constant
test_syntax("a = const(x)")

# redefined constant
test_syntax("A = const(1); A = const(2)")

# these operations are not supported within const
test_syntax("A = const(1 @ 2)")
test_syntax("A = const(1 << -2)")
test_syntax("A = const(1 >> -2)")
test_syntax("A = const(1 % 0)")
test_syntax("A = const(1 // 0)")
