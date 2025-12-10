try:
    from string.templatelib import Template, Interpolation
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    exec('t"test"')
except SyntaxError:
    print("SKIP")
    raise SystemExit

print("=== Parser error tests ===")
try:
    exec('t_empty = t"{}"')
    print(f"Empty expr: {t_empty.interpolations[0].value}")
except SyntaxError as e:
    print(f"Empty expr: SyntaxError - {e}")

# Whitespace in expression (MicroPython preserves, CPython strips trailing)
t_ws = t"{ 42 }"
print(f"Whitespace: {str(t_ws)}")

print("\n=== Error cases ===")
try:
    exec('t"{@}"')
except SyntaxError:
    print("Invalid syntax: SyntaxError")

try:
    long_expr = "x" * 10001
    exec(f't"{{{long_expr}}}"')
except (ValueError, SyntaxError, RuntimeError, NameError):
    print("Long expr: Error")

try:
    exec('t"hello" "world"')
except SyntaxError:
    print("Mixed concat: SyntaxError")

try:
    exec('bt"test"')
except SyntaxError:
    print("bt prefix: SyntaxError")

print("\n=== Escape sequence coverage tests ===")

def expect_invalid_escape(label, expr):
    try:
        eval(expr)
    except SyntaxError:
        print(f"{label}: SyntaxError")
    else:
        print(f"{label}: ERROR (expected SyntaxError)")

# Deprecated escape sequences (CPython warns, MicroPython accepts as literal)
print(repr(t"\8"))
print(repr(t"\9"))

# Unknown escape chars
print(repr(t"\z"))
print(repr(t"\k"))

# Invalid escapes
expect_invalid_escape("Invalid \\x escape", 't"\\xGG"')
expect_invalid_escape("Invalid \\u escape", 't"\\uGGGG"')
expect_invalid_escape("Invalid \\U escape", 't"\\UGGGGGGGG"')

# Unicode escapes (display format differs)
print(repr(t"\x00\x01\xFF"))
print(repr(t"\u0041"))
print(repr(t"\u03B1"))
print(repr(t"\u2764"))
print(repr(t"\U00000041"))
print(repr(t"\U0001F600"))
print(repr(t"\x41\u0042\103"))

# Unicode in triple-quoted strings
unicode_test = t"""Unicode test:
Emoji: {'\U0001f40d'}
Special: {'\u03b1 \u03b2 \u03b3'}"""
print(f"Unicode: {str(unicode_test)}")

print("\n=== Trailing whitespace preservation (PEP 750) ===")
x = 42
tmpl_trail = t"{x   }"
expr = tmpl_trail.interpolations[0].expression
print(f"Expression with trailing spaces: |{expr}|")
assert expr == "x   ", f"Expected 'x   ' but got '{expr}'"
assert len(expr) == 4, f"Expected length 4 but got {len(expr)}"

tmpl_both = t"{   x   }"
expr2 = tmpl_both.interpolations[0].expression
print(f"Expression with both spaces: |{expr2}|")
assert expr2 == "   x   ", f"Expected '   x   ' but got '{expr2}'"
assert len(expr2) == 7, f"Expected length 7 but got {len(expr2)}"

tmpl_lead = t"{   x}"
expr3 = tmpl_lead.interpolations[0].expression
print(f"Expression with leading spaces: |{expr3}|")
assert expr3 == "   x", f"Expected '   x' but got '{expr3}'"

# Debug specifiers: leading space preserved, trailing space stripped from expression
# PEP 750: "Whitespace is preserved in the debug specifier" (in strings part)
z = 99
t_debug4 = t"{z =}"
assert t_debug4.strings == ("z =", "")
assert t_debug4.interpolations[0].expression == "z"  # Trailing space stripped
assert t_debug4.interpolations[0].conversion == "r"

t_debug5 = t"{ z=}"
assert t_debug5.strings == (" z=", "")
assert t_debug5.interpolations[0].expression == " z"  # Leading space preserved
assert t_debug5.interpolations[0].conversion == "r"

t_debug6 = t"{ z =}"
assert t_debug6.strings == (" z =", "")
assert t_debug6.interpolations[0].expression == " z"  # Leading preserved, trailing stripped
assert t_debug6.interpolations[0].conversion == "r"

print("Trailing whitespace: PASS")

print("\n=== Error message tests ===")
try:
    exec('t"}"')
except SyntaxError:
    print("Lone }} rejected (correct)")

try:
    exec('t"{"')
except SyntaxError:
    print("Unterminated {{ rejected (correct)")

print("\n=== Triple quote empty interpolation ===")
try:
    exec('''empty_interp = t"""Start
{}
End"""''')
    print(f"Empty interpolation: {str(empty_interp)}")
except Exception as e:
    print(f"Empty interpolation error: {type(e).__name__}: {e}")

print("\nIncompatible tests completed!")
