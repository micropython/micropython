# function calls with *pos

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

# pos then iterator
foo(1, *range(2, 4))

# an iterator with many elements
def foo(*rest):
    print(rest)
foo(*range(10))

# method calls with *pos

class A:
    def foo(self, a, b, c):
        print(a, b, c)

a = A()
a.foo(*(1, 2, 3))
a.foo(1, *(2, 3))
a.foo(1, 2, *(3,))
a.foo(1, 2, 3, *())

# Another sequence type
a.foo(1, 2, *[100])

# Iterator
a.foo(*range(3))
