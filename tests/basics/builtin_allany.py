# test builtin "all" and "any"

tests = (
    (),
    [],
    [False],
    [True],
    [False, True],
    [True, False],
    [False, False],
    [True, True],
    (False for i in range(10)),
    (True for i in range(10)),
)

for test in tests:
    print(all(test))

for test in tests:
    print(any(test))
