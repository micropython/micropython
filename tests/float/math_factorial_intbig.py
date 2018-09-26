try:
    import math
    math.factorial
except (ImportError, NameError):
    print('SKIP')
    raise SystemExit(0)


for fun in (math.factorial,):
    for x in range(-1, 30):
        try:
            print('%d' % fun(x))
        except ValueError as e:
            print(str(e))
