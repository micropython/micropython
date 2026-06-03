"""
categories: Syntax,Annotations
description: MicroPython accepts type annotations on expressions where CPython forbids them.
cause: To reduce code size, MicroPython does not check the form of expressions with type annotations applied.
workaround: Always check for valid Python code using a linting tool.

The expressions themselves are not evaluated.
"""


def test(expr):
    code = f"def f():\n    {expr}: int"
    print(code)
    try:
        exec(code)
        print("OK")
    except SyntaxError as e:
        print("SyntaxError")
    print()


test("print('test')")
test("[x,y]")
test("x,y")
