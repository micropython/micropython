# test round() with integral values

tests = [
    [False], [True],
    [0], [1], [-1], [10],
    [1.23], [-1.78],
    [1.123456789, 0], [-1.123456789, 3], [1.123456789, 8]
]
for t in tests:
    print(round(*t))
