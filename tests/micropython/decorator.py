# test micropython-specific decorators

@micropython.bytecode
def f():
    return 'bytecode'

print(f())
