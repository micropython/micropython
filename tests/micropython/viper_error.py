# test syntax and type errors specific to viper code generation


def test(code, msg):
    try:
        exec(code)
    except (SyntaxError, ViperTypeError, NotImplementedError) as e:
        print(type(e), str(e) or msg)


# viper: annotations must be identifiers
test("@micropython.viper\ndef f(a:1): pass", "annotation must be an identifier")
test("@micropython.viper\ndef f() -> 1: pass", "annotation must be an identifier")

# unknown type
test("@micropython.viper\ndef f(x:unknown_type): pass", "unknown type 'unknown_type'")

# local used before type known
test(
    """
@micropython.viper
def f():
    print(x)
    x = 1
""",
    "local 'x' used before type known",
)

# type mismatch storing to local
test(
    """
@micropython.viper
def f():
    x = 1
    y = []
    x = y
""",
    "local 'x' has type 'int' but source is 'object'",
)

# can't implicitly convert type to bool
test(
    """
@micropython.viper
def f():
    x = ptr(0)
    if x:
        pass
""",
    "can't implicitly convert 'ptr' to 'bool'",
)

# incorrect return type
test("@micropython.viper\ndef f() -> int: return []", "return expected 'int' but got 'object'")

# can't do unary op of incompatible type
test("@micropython.viper\ndef f(x:ptr): -x", "can't do unary op of 'ptr'")

# can't do binary op between incompatible types
test("@micropython.viper\ndef f(): 1 + []", "can't do binary op between 'int' and 'object'")
test("@micropython.viper\ndef f(x:int, y:uint): x < y", "comparison of int and uint")

# can't load
test("@micropython.viper\ndef f(): 1[0]", "can't load from 'int'")
test("@micropython.viper\ndef f(): 1[x]", "can't load from 'int'")

# can't store
test("@micropython.viper\ndef f(): 1[0] = 1", "can't store to 'int'")
test("@micropython.viper\ndef f(): 1[x] = 1", "can't store to 'int'")
test("@micropython.viper\ndef f(x:int): x[0] = x", "can't store to 'int'")
test("@micropython.viper\ndef f(x:ptr32): x[0] = None", "can't store 'None'")
test("@micropython.viper\ndef f(x:ptr32): x[x] = None", "can't store 'None'")

# must raise an object
test("@micropython.viper\ndef f(): raise 1", "must raise an object")

# unary ops not implemented
test("@micropython.viper\ndef f(x:int): not x", "'not' not implemented")

# binary op not implemented
test("@micropython.viper\ndef f(x:uint, y:uint): res = x // y", "div/mod not implemented for uint")
test("@micropython.viper\ndef f(x:uint, y:uint): res = x % y", "div/mod not implemented for uint")
test("@micropython.viper\ndef f(x:int): res = x in x", "binary op  not implemented")

# raise with 0 or 2 args not implemented
test("@micropython.viper\ndef f():\n try:\n  x\n except:\n  raise\n", "native raise")
test("@micropython.viper\ndef f(): raise Exception from Exception", "native raise")

# yield (from) not implemented
test("@micropython.viper\ndef f(): yield", "native yield")
test("@micropython.viper\ndef f(): yield from f", "native yield")

# passing a ptr to a Python function not implemented
test("@micropython.viper\ndef f(): print(ptr(1))", "conversion to object")

# cast of a casting identifier not implemented
test("@micropython.viper\ndef f(): int(int)", "casting")
