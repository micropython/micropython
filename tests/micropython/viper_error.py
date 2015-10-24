# test syntax and type errors specific to viper code generation

def test(code):
    try:
        exec(code)
    except (SyntaxError, ViperTypeError) as e:
        print(repr(e))

# viper: annotations must be identifiers
test("@micropython.viper\ndef f(a:1): pass")
test("@micropython.viper\ndef f() -> 1: pass")

# local used before type known
test("""
@micropython.viper
def f():
    print(x)
    x = 1
""")

# type mismatch storing to local
test("""
@micropython.viper
def f():
    x = 1
    y = []
    x = y
""")

# can't implicitly convert type to bool
test("""
@micropython.viper
def f():
    x = ptr(0)
    if x:
        pass
""")

# incorrect return type
test("@micropython.viper\ndef f() -> int: return []")

# can't do binary op between incompatible types
test("@micropython.viper\ndef f(): 1 + []")

# can't load
test("@micropython.viper\ndef f(): 1[0]")
test("@micropython.viper\ndef f(): 1[x]")

# can't store
test("@micropython.viper\ndef f(): 1[0] = 1")
test("@micropython.viper\ndef f(): 1[x] = 1")

# must raise an object
test("@micropython.viper\ndef f(): raise 1")

# unary ops not implemented
test("@micropython.viper\ndef f(x:int): +x")
test("@micropython.viper\ndef f(x:int): -x")
test("@micropython.viper\ndef f(x:int): ~x")
