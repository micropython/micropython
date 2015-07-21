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
# TODO uPy currently only supports second arg being 0
print(round(1.4, 0))
