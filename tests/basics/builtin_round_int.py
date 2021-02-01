# test round() with integer values and second arg

# rounding integers is an optional feature so test for it
try:
    round(1, -1)
except NotImplementedError:
    print('SKIP')
    raise SystemExit

tests = [
    (1, False), (1, True),
    (124, -1), (125, -1), (126, -1),
    (5, -1), (15, -1), (25, -1),
    (12345, 0), (12345, -1), (12345, 1),
    (-1234, 0), (-1234, -1), (-1234, 1),
]
for t in tests:
    print(round(*t))
