# overriding default arguments

def foo(a, b=3):
    print(a, b)

# override with positional
foo(1, 333)

# override with keyword
foo(1, b=333)

# override with keyword
foo(a=2, b=333)

def foo2(a=1, b=2):
    print(a, b)

# default and keyword
foo2(b='two')
