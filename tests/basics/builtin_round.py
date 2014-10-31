# test round()

# check basic cases
tests = [
    False, True,
    0, 1, -1, 10,
    0.0, 1.0, 0.1, -0.1, 123.4, 123.6, -123.4, -123.6
]
for t in tests:
    print(round(t))

# check .5 cases
for i in range(11):
    print(round((i - 5) / 2))
