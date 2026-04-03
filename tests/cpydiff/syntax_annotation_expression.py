"""
categories: Syntax,Annotations
description: Type annotations allowed only on identifiers
cause: Micropython disallows annotating certain expressions that CPython allows. These disallowed annotations are rejected as invalid by common type checkers.
workaround: Remove the annotation.
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


test("x.y")
test("x.y.z")
test("x[1]")
test("x[1].y")
