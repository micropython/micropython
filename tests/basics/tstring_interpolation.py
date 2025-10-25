try:
    from string.templatelib import Template, Interpolation
except ImportError:
    print("SKIP")
    raise SystemExit

# Check if t-strings are supported
try:
    exec('t"test"')
except SyntaxError:
    print("SKIP")
    raise SystemExit

try:
    import gc
    import sys
    gc.collect()
except (ImportError, MemoryError):
    print("SKIP")
    raise SystemExit

print("\n=== Bracket/paren depth tracking ===")
d = {'a': 1, 'b:c': 2}
items = [10, 20, 30]
colon_key = t"{d['b:c']}"
print(f"Colon in key: expr={colon_key.interpolations[0].expression}, value={colon_key.interpolations[0].value}")

slice_expr = t"{items[1:3]}"
print(f"Slice colon: expr={slice_expr.interpolations[0].expression}, value={slice_expr.interpolations[0].value}")

def pair(x, y=':'):
    return (x, y)

func_expr = t"{pair(1, 2)}"
print(f"Function call: expr={func_expr.interpolations[0].expression}, value={func_expr.interpolations[0].value}")

default_expr = t"{pair(3)}"
print(f"Default arg colon: expr={default_expr.interpolations[0].expression}, value={default_expr.interpolations[0].value}")

matrix = [[1, 2], [3, 4]]
nested_expr = t"{matrix[0][1]}"
print(f"Nested brackets: expr={nested_expr.interpolations[0].expression}, value={nested_expr.interpolations[0].value}")


print("\n=== Additional coverage tests ===")
print("\n=== Inline template literal tests ===")

try:
    result = t"outer { t'' }"
    print(f"Empty nested: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"Empty nested: FAIL - {type(e).__name__}: {e}")

try:
    result = t"outer { t'inner' }"
    print(f"With content: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"With content: FAIL - {type(e).__name__}: {e}")

try:
    result = t"result: { t'a' + t'b' }"
    print(f"Concatenation: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"Concatenation: FAIL - {type(e).__name__}: {e}")

try:
    result = t'Single { t"double" }'
    print(f"Mixed quotes: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"Mixed quotes: FAIL - {type(e).__name__}: {e}")

try:
    x = 42
    result = t"outer { t'inner {x}' }"
    print(f"Nested interp: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"Nested interp: FAIL - {type(e).__name__}: {e}")

print("\n=== Integer overflow test ===")
print("\n=== Escape in string within interpolation ===")
try:
    code = r"result = t'{\"test\"value\"}'"
    exec(code)
    print(f"Escaped quote in string: OK")
except Exception as e:
    print(f"Escaped quote error: {type(e).__name__}")

try:
    code = r"result = t'{\"\\n\"}'"
    exec(code)
    print(f"Escaped newline in string: OK")
except Exception as e:
    print(f"Escaped newline error: {type(e).__name__}")

print("\n=== Format spec with special characters ===")
