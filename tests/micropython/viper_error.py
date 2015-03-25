# test syntax errors specific to viper code generation

def test_syntax(code):
    try:
        exec(code)
    except SyntaxError:
        print("SyntaxError")

# viper: annotations must be identifiers
test_syntax("@micropython.viper\ndef f(a:1): pass")
test_syntax("@micropython.viper\ndef f() -> 1: pass")
