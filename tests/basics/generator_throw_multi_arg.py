# Test generator .throw() with multiple arguments.
# Using multiple arguments is deprecated since CPython 3.12.

# Generator ignores a thrown GeneratorExit (this is allowed).
def gen():
    try:
        yield 123
    except GeneratorExit as e:
        print("GeneratorExit", repr(e.args))
    yield 456


# Thrown an instance with None as second arg.
g = gen()
print(next(g))
print(g.throw(GeneratorExit(), None))

# Thrown a class and instance.
g = gen()
print(next(g))
print(g.throw(GeneratorExit, GeneratorExit(123)))
