# test round() with floats

# check basic cases
tests = [
    [0.0], [1.0], [0.1], [-0.1], [123.4], [123.6], [-123.4], [-123.6],
    [1.234567, 5], [1.23456, 1], [1.23456, 0], [1234.56, -2]
]
for t in tests:
    print(round(*t))

# check .5 cases
for i in range(11):
    print(round((i - 5) / 2))

# test second arg
for i in range(-1, 3):
    print(round(1.47, i))

# test inf and nan
for val in (float('inf'), float('nan')):
    try:
        round(val)
    except (ValueError, OverflowError) as e:
        print(type(e))
