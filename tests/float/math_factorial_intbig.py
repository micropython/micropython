try:
    import math

    math.factorial
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


for fun in (math.factorial,):
    for x in range(-1, 30):
        try:
            print("%d" % fun(x))
        except ValueError as e:
            print("ValueError")
