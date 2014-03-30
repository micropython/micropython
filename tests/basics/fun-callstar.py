def foo(a, b, c):
    print(a, b, c)

foo(*(1, 2, 3))
foo(1, *(2, 3))
foo(1, 2, *(3,))
foo(1, 2, 3, *())

# Another sequence type
foo(1, 2, *[100])

# Iterator
foo(*range(3))
