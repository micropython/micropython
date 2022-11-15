# Test behaviour of inf and nan in basic float operations

inf = float("inf")
nan = float("nan")

values = (-2, -1, 0, 1, 2, inf, nan)

for x in values:
    for y in values:
        print(x, y)
        print("  + - *", x + y, x - y, x * y)
        try:
            print("  /", x / y)
        except ZeroDivisionError:
            print("  / ZeroDivisionError")
        try:
            print("  ** pow", x**y, pow(x, y))
        except ZeroDivisionError:
            print("  ** pow ZeroDivisionError")
        print("  == != < <= > >=", x == y, x != y, x < y, x <= y, x > y, x >= y)
