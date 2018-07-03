# test round() with large integer values and second arg

# rounding integers is an optional feature so test for it
try:
    round(1, -1)
except NotImplementedError:
    print('SKIP')
    raise SystemExit

i = 2**70

tests = [
    (i, 0), (i, -1), (i, -10), (i, 1),
    (-i, 0), (-i, -1), (-i, -10), (-i, 1),
]
for t in tests:
    print(round(*t))
