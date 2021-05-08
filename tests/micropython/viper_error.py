# test syntax and type errors specific to viper code generation


def test(code):
    try:
        exec(code)
    except (SyntaxError, ViperTypeError, NotImplementedError) as e:
        print(repr(e))


# viper: annotations must be identifiers
test("@micropython.viper\ndef f(a:1): pass")
test("@micropython.viper\ndef f() -> 1: pass")

# unknown type
test("@micropython.viper\ndef f(x:unknown_type): pass")

# local used before type known
test(
    """
@micropython.viper
def f():
    print(x)
    x = 1
"""
)

# type mismatch storing to local
test(
    """
@micropython.viper
def f():
    x = 1
    y = []
    x = y
"""
)

# can't implicitly convert type to bool
test(
    """
@micropython.viper
def f():
    x = ptr(0)
    if x:
        pass
"""
)

# incorrect return type
test("@micropython.viper\ndef f() -> int: return []")

# can't do binary op between incompatible types
test("@micropython.viper\ndef f(): 1 + []")
test("@micropython.viper\ndef f(x:int, y:uint): x < y")

# can't load
test("@micropython.viper\ndef f(): 1[0]")
test("@micropython.viper\ndef f(): 1[x]")

# can't store
test("@micropython.viper\ndef f(): 1[0] = 1")
test("@micropython.viper\ndef f(): 1[x] = 1")
test("@micropython.viper\ndef f(x:int): x[0] = x")
test("@micropython.viper\ndef f(x:ptr32): x[0] = None")
test("@micropython.viper\ndef f(x:ptr32): x[x] = None")

# must raise an object
test("@micropython.viper\ndef f(): raise 1")

# unary ops not implemented
test("@micropython.viper\ndef f(x:int): +x")
test("@micropython.viper\ndef f(x:int): -x")
test("@micropython.viper\ndef f(x:int): ~x")

# binary op not implemented
test("@micropython.viper\ndef f(x:uint, y:uint): res = x // y")
test("@micropython.viper\ndef f(x:uint, y:uint): res = x % y")
test("@micropython.viper\ndef f(x:int): res = x in x")

# yield (from) not implemented
test("@micropython.viper\ndef f(): yield")
test("@micropython.viper\ndef f(): yield from f")

# passing a ptr to a Python function not implemented
test("@micropython.viper\ndef f(): print(ptr(1))")

# cast of a casting identifier not implemented
test("@micropython.viper\ndef f(): int(int)")
