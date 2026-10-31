# Test freezing functions with positional-only / syntax.


def f(a, /, b):
    return a, b


g = lambda a, /, b: (a, b)

for func in f, g:
    # Call with positional args.
    print(func(1, 2))

    # Call positional-only with keyword (should fail).
    try:
        func(a=1, b=2)
    except TypeError:
        print("TypeError")

    # Call positional-only with empty key in **dict (should fail).
    try:
        func(**{"": 1, "b": 2})
    except TypeError:
        print("TypeError")
