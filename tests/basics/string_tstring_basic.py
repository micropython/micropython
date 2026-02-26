try:
    from string.templatelib import Template, Interpolation
except ImportError:
    print("SKIP")
    raise SystemExit

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
# print(f"ascii: {str(t'{val!a}')}")

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

print("\n=== Escaped braces evaluation ===")
t_escaped = Template("{", Interpolation(42, "x"), "}", Interpolation(42, "x"), "{{", Interpolation(42, "x"), "}}")
print(f"Escaped eval: '{str(t_escaped)}'")

t_braces = Template("{{hello}}", Interpolation(1, "a"), " {", Interpolation(2, "b"), "} ", Interpolation(3, "c"), "{{world}}")
print(f"Braces in strings: '{str(t_braces)}'")

print("\n=== Memory stress test ===")
for n in [10, 20, 30]:
    args = []
    for i in range(n):
        args.append("s")
        args.append(Interpolation(i, f"var{i}"))
    args.append("s")
    t_mem = Template(*args)
    result = str(t_mem)
    print(f"Memory test [{n}]: {len(result)} chars")

large_args = []
for i in range(20):
    large_args.append("")
    large_args.append(Interpolation(i, f"v{i}"))
large_args.append("")
t_large = Template(*large_args)
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

t_escaped1 = t'{{""}}'
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

print("\n=== Escape sequence tests ===")
print(repr(t"Line1\nLine2"))
print(repr(t"Path\\to\\file"))
print(repr(t"It\'s working"))
print(repr(t"She said \"Hello\""))
print(repr(t"Bell\a"))
print(repr(t"Back\bspace"))
print(repr(t"Tab\there"))
print(repr(t"Vertical\vtab"))
print(repr(t"Form\ffeed"))
print(repr(t"Carriage\rreturn"))

# Valid hex escapes
print(repr(t"\x41"))
print(repr(t"\x7F"))

# Valid octal escapes
print(repr(t"\0"))
print(repr(t"\7"))
print(repr(t"\77"))
print(repr(t"\101"))

print(repr(t"\1a"))
print(repr(t"\123c"))

# Line continuation
print(repr(t"First \
second"))
print(repr(t"One\
Two\
Three"))

# Multiple escapes
print(repr(t"\n\t\r"))

# Raw strings
print(repr(rt"\n\t\x41"))
print(repr(rt"C:\new\test"))

# Triple quoted strings
print(repr(t"""\n\t\x41"""))
print(repr(rt"""\n\t\x41"""))

print("\n=== Triple quote edge cases ===")
doc = '''This is a
multi-line
docstring'''
result1 = t"""Documentation:
{doc}
End of doc"""
print(f"Triple in interpolation: {str(result1)}")
print(f"   Value: {repr(result1.interpolations[0].value)}")

data = {
    "users": [
        {"name": "Alice", "msg": 'Say "Hello"'},
        {"name": "Bob", "msg": "It's great!"}
    ]
}
complex_nested = t"""Users:
{data['users'][0]['name']}: {data['users'][0]['msg']}
{data['users'][1]['name']}: {data['users'][1]['msg']}"""
print(f"Complex nested: {str(complex_nested)}")
print(f"   Values: {complex_nested.values}")

import os
path_sep = os.sep
raw_with_interp = rt"""Path: C:\Users\{path_sep}Documents
Raw newline: \n
Raw tab: \t"""
print(f"Raw with interpolation: {str(raw_with_interp)}")
print(f"   String 0 repr: {repr(raw_with_interp.strings[0])}")

long_line = "x" * 50
long_triple = t"""Start
{long_line}
End"""
print(f"Long line: strings={long_triple.strings}")
print(f"   Value length: {len(long_triple.values[0])}")

inner = t"inner {42}"
outer = t"""Outer template:
{inner}
End"""
print(f"Nested templates: {str(outer)}")
print(f"   Inner value: {outer.values[0]}")

formatted = t"""Math constants:
Pi: {314:.2f}
E: {271:.1e}
Sqrt(2): {141:.0f}"""
print(f"Formatted values: {repr(formatted)}")
print(f"    Interpolation count: {len(formatted.interpolations)}")
print(f"    Format specs: {[i.format_spec for i in formatted.interpolations]}")

x, y, z = 1, 2, 3
debug_complex = t"""Debug info:
{x + y=} {x * y * z=} {x < y < z=}"""
print(f"Debug complex: {repr(debug_complex)}")
print(f"    Expressions: {[i.expression for i in debug_complex.interpolations]}")

part1 = t"""Part 1
with newline"""
part2 = t'''Part 2
also multiline'''
concatenated = part1 + part2
print(f"Concatenated triple: {repr(concatenated)}")
print(f"    Strings: {concatenated.strings}")

print("\n=== PEP 701 brace compliance ===")
t_brace1 = t"{{"
print(f"Escaped {{{{: {t_brace1.strings}")
assert t_brace1.strings == ('{',)

t_brace2 = t"}}"
print(f"Escaped }}}}: {t_brace2.strings}")
assert t_brace2.strings == ('}',)

t_brace3 = t"test{{escape}}here"
print(f"Mixed: {t_brace3.strings}")
assert t_brace3.strings == ('test{escape}here',)

print("\n=== Format spec scope resolution ===")
def test_format_scope():
    width = 10
    x = 42
    t_scope = t"{x:{width}}"
    print(f"Local vars: width={width}, x={x}")
    print(f"Format spec: {t_scope.interpolations[0].format_spec}")
    return t_scope.interpolations[0].format_spec == '10'

result = test_format_scope()
print(f"Scope test: {'PASS' if result else 'FAIL'}")
assert result, "Format spec scope resolution failed"

precision = 2
value = 3.14159
t_prec = t"{value:.{precision}f}"
print(f"Precision: format_spec={t_prec.interpolations[0].format_spec}")
assert t_prec.interpolations[0].format_spec == '.2f'

print("\nBasic tests completed!")
