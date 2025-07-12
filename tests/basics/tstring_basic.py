from string.templatelib import Template, Interpolation

print("=== Basic functionality ===")
t = t"Hello World"
print(type(t).__name__)

name = "World"
t2 = t"Hello {name}"
print(f"Strings: {t2.strings}")
print(f"Value: {t2.interpolations[0].value}")

print(f"str(): {str(t2)}")

t_raw = rt"Path: C:\test\{name}"
print(f"Raw: '{t_raw.strings[0]}'")

t_tr = tr"Path: C:\test\{name}"
print(f"tr: '{t_tr.strings[0]}'")

print("\n=== Parser tests ===")
try:
    exec('t_empty = t"{}"')
    print(f"Empty expr: {t_empty.interpolations[0].value}")
except SyntaxError as e:
    print(f"Empty expr: SyntaxError - {e}")

t_ws = t"{ 42 }"
print(f"Whitespace: {str(t_ws)}")

data = {"a": {"b": [1, 2, 3]}}
t_complex = t"{data['a']['b'][0]}"
print(f"Complex: {str(t_complex)}")

print(f"None: {str(t'{None}')}")
print(f"True: {str(t'{True}')}")
print(f"False: {str(t'{False}')}")
print(f"Ellipsis: {str(t'{...}')}")

obj = type('Obj', (), {
    '__getattr__': lambda s, n: s,
    '__getitem__': lambda s, k: s,
    '__call__': lambda s, *a: 42,
    '__str__': lambda s: "42"
})()
print(f"Deep nest: {str(t'{obj.a.b[0].c()}')}")

print("\n=== Conversions and formatting ===")
val = {"key": "value"}
print(f"repr: {str(t'{val!r}')}")
print(f"str: {str(t'{val!s}')}")
print(f"ascii: {str(t'{val!a}')}")

print(f"Width: '{str(t'{42:10d}')}'")
print(f"Precision: {str(t'{3.14159:.2f}')}")

x = 42
t_debug = t"{x=}"
assert t_debug.strings == ("x=", "")
assert t_debug.interpolations[0].expression == "x"
assert t_debug.interpolations[0].conversion == "r"
assert t_debug.interpolations[0].value == 42
print(f"Debug: {str(t_debug)}")

y = 10
t_debug2 = t"{x + y=}"
assert t_debug2.strings == ("x + y=", "")
assert t_debug2.interpolations[0].expression == "x + y"
assert t_debug2.interpolations[0].conversion == "r"
assert t_debug2.interpolations[0].value == 52

pi = 3.14159
t_debug3 = t"{pi=:.2f}"
assert t_debug3.strings == ("pi=", "")
assert t_debug3.interpolations[0].expression == "pi"
assert t_debug3.interpolations[0].conversion is None
assert t_debug3.interpolations[0].format_spec == ".2f"
assert t_debug3.interpolations[0].value == 3.14159

z = 99
t_debug4 = t"{z =}"
assert t_debug4.strings == ("z =", "")
assert t_debug4.interpolations[0].expression == "z"
assert t_debug4.interpolations[0].conversion == "r"

t_debug5 = t"{ z=}"
assert t_debug5.strings == (" z=", "")
assert t_debug5.interpolations[0].expression == " z"
assert t_debug5.interpolations[0].conversion == "r"

t_debug6 = t"{ z =}"
assert t_debug6.strings == (" z =", "")
assert t_debug6.interpolations[0].expression == " z"
assert t_debug6.interpolations[0].conversion == "r"

print("\n=== Constructor tests ===")
t_empty = Template()
print(f"Empty: {t_empty.strings}")

t_single = Template("single")
print(f"Single: {t_single.strings}")

t_multi = Template("a", "b", "c")
print(f"Multi: {t_multi.strings}")

i1 = Interpolation(1, "x")
i2 = Interpolation(2, "y")
t_mixed = Template("start", i1, "middle", i2, "end")
print(f"Mixed: strings={t_mixed.strings}, values={t_mixed.values}")

print("\n=== Operations ===")
t1 = t"Hello"
t2 = t" World"
t_concat = t1 + t2
print(f"Concat: '{str(t_concat)}'")

items = list(t"a{1}b{2}c")
print(f"Iterator: {[type(x).__name__ for x in items]}")

t_attr = t"test{42}"
print(f"strings attr: {t_attr.strings}")
print(f"interpolations attr: {len(t_attr.interpolations)}")
print(f"values attr: {t_attr.values}")

print(f"repr: {repr(t2)[:50]}...")

print("\n=== Error cases ===")
try:
    exec('t"{@}"')
except SyntaxError:
    print("Invalid syntax: SyntaxError")

try:
    long_expr = "x" * 10001
    exec(f't"{{{long_expr}}}"')
except (ValueError, SyntaxError, RuntimeError):
    print("Long expr: Error")

try:
    exec('t"hello" "world"')
except SyntaxError:
    print("Mixed concat: SyntaxError")

try:
    exec('bt"test"')
except SyntaxError:
    print("bt prefix: SyntaxError")


print("\n=== Escaped braces evaluation ===")
t_escaped = Template(("{", "}", "{{", "}}"), (Interpolation(42, "x"),) * 3)
print(f"Escaped eval: '{str(t_escaped)}'")

t_braces = Template(("{{hello}}", " {", "} ", "{{world}}"), 
                   (Interpolation(1, "a"), Interpolation(2, "b"), Interpolation(3, "c")))
print(f"Braces in strings: '{str(t_braces)}'")

print("\n=== Memory stress test ===")
for n in [10, 20, 30]:
    interps = tuple(Interpolation(i, f"var{i}") for i in range(n))
    strings = ("s",) * (n + 1)
    t_mem = Template(strings, interps)
    result = str(t_mem)
    print(f"Memory test [{n}]: {len(result)} chars")

large_interps = tuple(Interpolation(i, f"v{i}") for i in range(20))
large_strings = ("",) * 21
t_large = Template(large_strings, large_interps)
print(f"Large values: {len(t_large.values)} values")

print("\n=== Nested quotes and braces tests ===")
t_nested1 = t"{"{}"}"
print(f"Nested quotes 1: {str(t_nested1)}")
print(f"  Value: {t_nested1.interpolations[0].value}")
print(f"  Expression: {t_nested1.interpolations[0].expression}")

t_nested2 = t"{'hello'}"
print(f"Nested quotes 2: {str(t_nested2)}")
print(f"  Value: {t_nested2.interpolations[0].value}")

t_nested3 = t'{"world"}'
print(f"Nested quotes 3: {str(t_nested3)}")
print(f"  Value: {t_nested3.interpolations[0].value}")

t_nested4 = t"{['a', 'b', 'c'][1]}"
print(f"Nested quotes 4: {str(t_nested4)}")
print(f"  Value: {t_nested4.interpolations[0].value}")

d = {"key": "value", "x": 123}
t_nested5 = t"{d['key']}"
print(f"Nested quotes 5: {str(t_nested5)}")
print(f"  Value: {t_nested5.interpolations[0].value}")

t_escaped1 = t"{{""}}"
print(f"Escaped braces 1: {str(t_escaped1)}")
print(f"  Strings: {t_escaped1.strings}")

t_escaped2 = t"{{}}}}"
print(f"Escaped braces 2: {str(t_escaped2)}")
print(f"  Strings: {t_escaped2.strings}")

x = "test"
t_mixed = t"{{before}} {x} {{after}}"
print(f"Mixed escaped: {str(t_mixed)}")
print(f"  Strings: {t_mixed.strings}")
print(f"  Value: {t_mixed.interpolations[0].value}")

t_escape = t"{'\n\t'}"
print(f"Escape sequences: {str(t_escape)}")
print(f"  Value repr: {repr(t_escape.interpolations[0].value)}")

inner = "{}"
t_nested_expr = t"{inner}"
print(f"Nested expr: {str(t_nested_expr)}")
print(f"  Value: {t_nested_expr.interpolations[0].value}")

print("\n=== Interpolation attribute tests ===")
i_basic = Interpolation(42, "x")
print(f"Basic conversion: {i_basic.conversion}")
print(f"Basic format_spec: {i_basic.format_spec}")

i_with_conv = Interpolation(42, "x", "s")
print(f"With conversion: {i_with_conv.conversion}")

i_with_fmt = Interpolation(42, "x", None, ":>10")
print(f"With format_spec: {i_with_fmt.format_spec}")

i_full = Interpolation(42, "x", "r", ":>10")
print(f"Full conversion: {i_full.conversion}")
print(f"Full format_spec: {i_full.format_spec}")

t_conv = t"{42!s}"
print(f"Template conversion: {t_conv.interpolations[0].conversion}")

t_fmt = t"{42:>10}"
print(f"Template format_spec: {t_fmt.interpolations[0].format_spec}")

t_both = t"{42!r:>10}"
print(f"Both conversion: {t_both.interpolations[0].conversion}")
print(f"Both format_spec: {t_both.interpolations[0].format_spec}")

print("\nBasic tests completed!")


print("\n=== Triple quote edge cases ===")

doc = '''This is a
multi-line
docstring'''
result1 = t"""Documentation:
{doc}
End of doc"""
print(f"1. Triple in interpolation: {str(result1)}")
print(f"   Value: {repr(result1.interpolations[0].value)}")

try:
    exec('''empty_interp = t"""Start
{}
End"""''')
    print(f"\n4. Empty interpolation: {str(empty_interp)}")
except Exception as e:
    print(f"\n4. Empty interpolation error: {type(e).__name__}: {e}")

data = {
    "users": [
        {"name": "Alice", "msg": 'Say "Hello"'},
        {"name": "Bob", "msg": "It's great!"}
    ]
}
complex = t"""Users:
{data['users'][0]['name']}: {data['users'][0]['msg']}
{data['users'][1]['name']}: {data['users'][1]['msg']}"""
print(f"\n5. Complex nested: {str(complex)}")
print(f"   Values: {complex.values}")

unicode_test = t"""Unicode test:
Emoji: {'🐍'}
Special: {'α β γ'}"""
print(f"\n6. Unicode: {str(unicode_test)}")

import os
path_sep = os.sep
raw_with_interp = rt"""Path: C:\Users\{path_sep}Documents
Raw newline: \n
Raw tab: \t"""
print(f"\n7. Raw with interpolation: {str(raw_with_interp)}")
print(f"   String 0 repr: {repr(raw_with_interp.strings[0])}")

long_line = "x" * 50
long_triple = t"""Start
{long_line}
End"""
print(f"\n8. Long line: strings={long_triple.strings}")
print(f"   Value length: {len(long_triple.values[0])}")

inner = t"inner {42}"
outer = t"""Outer template:
{inner}
End"""
print(f"\n9. Nested templates: {str(outer)}")
print(f"   Inner value: {outer.values[0]}")

formatted = t"""Math constants:
Pi: {314:.2f}
E: {271:.1e}
Sqrt(2): {141:.0f}"""
print(f"\n10. Formatted values: {repr(formatted)}")
print(f"    Interpolation count: {len(formatted.interpolations)}")
print(f"    Format specs: {[i.format_spec for i in formatted.interpolations]}")

x, y, z = 1, 2, 3
debug_complex = t"""Debug info:
{x + y=} {x * y * z=} {x < y < z=}"""
print(f"\n11. Debug complex: {repr(debug_complex)}")
print(f"    Expressions: {[i.expression for i in debug_complex.interpolations]}")

part1 = t"""Part 1
with newline"""
part2 = t'''Part 2
also multiline'''
concatenated = part1 + part2
print(f"\n12. Concatenated triple: {repr(concatenated)}")
print(f"    Strings: {concatenated.strings}")

print("\nTriple quote edge case tests completed!")