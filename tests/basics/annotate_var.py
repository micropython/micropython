# test PEP 526, variable annotations

x: int
print("x" in globals())

x: int = 1
print(x)

t: tuple = 1, 2
print(t)

# a pure annotation in a function makes that variable local
def f():
    x: int
    try:
        print(x)
    except NameError:
        print("NameError")
f()

# An annotation like `f(): int` is correctly rejected
try:
    eval("def f():\n    f(): int")
    print("OK")
except SyntaxError as e:
    print("SyntaxError")

# Note: Annotations like `x.y: int` are not tested here, see cpydiff/syntax_annotation_expression.py
