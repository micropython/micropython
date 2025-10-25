from string.templatelib import Template, Interpolation

print("=== Basic functionality ===")
t = t"Hello World"
print(type(t).__name__)

name = "World"
t2 = t"Hello {name}"
print(f"Strings: {t2.strings}")
print(f"Value: {t2.interpolations[0].value}")

t_raw = rt"Path: C:\test\{name}"
print(f"Raw: '{t_raw.strings[0]}'")

t_tr = tr"Path: C:\test\{name}"
print(f"tr: '{t_tr.strings[0]}'")

print("\n=== Conversions and formatting ===")
val = {"key": "value"}
print(f"repr: {str(t'{val!r}')}")
print(f"ascii: {str(t'{val!a}')}")

print(f"Width: '{str(t'{42:10d}')}'")
print(f"Precision: {str(t'{3.14159:.2f}')}")

x = 42
t_debug = t"{x=}"
assert t_debug.strings == ("x=", "")
assert t_debug.interpolations[0].conversion == "r"
print(f"Debug: {str(t_debug)}")

print("\n=== Constructor tests ===")
t_empty = Template()
print(f"Empty: {t_empty.strings}")

t_single = Template("single")
print(f"Single: {t_single.strings}")

i1 = Interpolation(1, "x")
t_mixed = Template("start", i1, "end")
print(f"Mixed: strings={t_mixed.strings}, values={t_mixed.values}")

print("\n=== Operations ===")
t1 = t"Hello"
t2 = t" World"
t_concat = t1 + t2
print(f"Concat: '{str(t_concat)}'")

items = list(t"a{1}b{2}c")
print(f"Iterator: {[type(x).__name__ for x in items]}")

print("\n=== Error cases ===")
try:
    exec('t"{@}"')
except SyntaxError:
    print("Invalid syntax: SyntaxError")

try:
    exec('t"hello" "world"')
except SyntaxError:
    print("Mixed concat: SyntaxError")

print("\n=== Nested quotes ===")
t_nested = t"{"{}"}"
print(f"Nested: {str(t_nested)}")

d = {"key": "value"}
t_dict = t"{d['key']}"
print(f"Dict: {str(t_dict)}")

print("\nBasic tests completed!")
