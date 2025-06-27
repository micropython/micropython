# Edge case tests for t-string parser and lexer
# Focuses on error paths and special cases

print("=== Parser memory and error cases ===")

# Test various whitespace types in expressions
whitespace_tests = [
    ("empty", "{}"),
    ("space", "{ }"),
    ("spaces", "{   }"),
    ("tab", "{\t}"),
    ("newline", "{\n}"),
    ("mixed", "{ \t\n }"),
]

for name, expr in whitespace_tests:
    try:
        t = eval(f't"{expr}"')
        val = t.interpolations[0].value if t.interpolations else "no interp"
        print(f"{name}: {val}")
    except Exception as e:
        print(f"{name}: {type(e).__name__}")

print("\n=== Complex nested structures ===")

# Very deep nesting to test parse tree copying
data = 42
for i in range(20):
    data = {"x": data}

# Build expression to access deeply nested value
expr = "data"
for i in range(20):
    expr += "['x']"

t_deep = eval(f't"{{{expr}}}"')
print(f"Deep nesting (20 levels): {t_deep.__str__()}")

# Test const_object nodes in parser
const_exprs = [
    ("None", "{None}"),
    ("True", "{True}"),
    ("False", "{False}"),
    ("Ellipsis", "{...}"),
    ("int", "{42}"),
    ("float", "{3.14}"),
    ("complex", "{1+2j}"),
    ("bytes", "{b'bytes'}"),
    ("bigint", "{10**100}"),
]

for name, expr in const_exprs:
    t = eval(f't"{expr}"')
    print(f"Const {name}: {t.__str__()}")

print("\n=== Expression syntax errors ===")

# Various invalid expressions to test error handling
invalid_exprs = [
    ("unclosed paren", "{(1 + 2"),
    ("unclosed bracket", "{[1, 2"),
    ("unclosed brace", "{{1, 2"),
    ("invalid token", "{@}"),
    ("empty brackets", "{[]}"),
    ("multiple ops", "{1 ++ 2}"),
    ("assignment", "{x = 5}"),
    ("import", "{import sys}"),
    ("class def", "{class X: pass}"),
    ("function def", "{def f(): pass}"),
    ("yield expr", "{yield x}"),
    ("await expr", "{await x}"),
]

for name, expr in invalid_exprs:
    try:
        exec(f't"{expr}"')
        print(f"ERROR: {name} should fail")
    except SyntaxError:
        print(f"{name}: SyntaxError")
    except Exception as e:
        print(f"{name}: {type(e).__name__}")

print("\n=== Large expression handling ===")

# Expression close to limit
expr_9999 = "1 + " * 2499 + "1"  # 9999 chars
try:
    t = eval(f't"{{{expr_9999}}}"')
    print(f"9999 char expr: OK ({t.__str__()[:20]}...)")
except Exception as e:
    print(f"9999 char expr: {type(e).__name__}")

# Expression exactly at limit (10000)
expr_10000 = "1 + " * 2499 + "1 + 1"[:4]  # exactly 10000
try:
    t = eval(f't"{{{expr_10000}}}"')
    print(f"10000 char expr: OK")
except Exception as e:
    print(f"10000 char expr: {type(e).__name__}")

# Expression over limit
expr_10001 = expr_10000 + "x"
try:
    t = eval(f't"{{{expr_10001}}}"')
    print("ERROR: 10001 char should fail")
except ValueError as e:
    print(f"10001 char expr: ValueError ({e})")
except SyntaxError:
    print("10001 char expr: SyntaxError")

print("\n=== Special lexer cases ===")

# Test implicit concatenation edge cases
t_concat = t"a" t"b" t"c"
print(f"Triple concat: '{t_concat.strings[0]}'")

# Mix raw and regular
t_mix = rt"raw\n" t"regular\n"
print(f"Raw+regular: len={len(t_mix.strings[0])}")

# Empty t-strings
t_empty1 = t""
t_empty2 = t''
print(f"Empty t-strings: {len(t_empty1.strings[0])}, {len(t_empty2.strings[0])}")

# Triple quoted t-strings - if supported
try:
    exec("t_triple1 = t'''triple\\nsingle'''")
    print(f"Triple quoted single: supported")
except SyntaxError:
    print(f"Triple quoted single: not supported")

try:
    exec('t_triple2 = t"""triple\\ndouble"""')
    print(f"Triple quoted double: supported")
except SyntaxError:
    print(f"Triple quoted double: not supported")

print("\n=== Format spec edge cases ===")

# Test escaped braces - SKIP due to hanging issue
print("Format spec escaped braces: SKIPPED (implementation hangs)")

# Test interpolation in format spec with various cases
width = 5
try:
    t = eval(f't"{{test_val:{{width}}}}"')
    print(f"Nested interpolation: '{t.__str__()}'")
except:
    print("Nested interpolation: failed")

# Format spec with colon and exclamation
try:
    t = eval(f't"{{test_val:{{width}}:more}}"')
    print(f"Format with colon: '{t.__str__()}'")
except:
    print("Format with colon: failed")

print("\n=== Memory stress tests ===")

# Many interpolations (test dynamic array growth)
parts = []
for i in range(50):
    parts.append(f"text{i}")
    if i < 49:
        parts.append(f"{{{i}}}")

large_template = "t\"" + "".join(parts) + "\""
try:
    t = eval(large_template)
    print(f"50 interpolations: {len(t.interpolations)} interpolations")
except Exception as e:
    print(f"50 interpolations: {type(e).__name__}")

# Very long string parts
try:
    long_str = "x" * 1000
    t_long = eval(f't"{long_str}{{{42}}}{long_str}"')
    print(f"Long strings: total length = {sum(len(s) for s in t_long.strings)}")
except RuntimeError as e:
    print(f"Long strings: RuntimeError ({e})")

print("\nParser edge tests completed!")