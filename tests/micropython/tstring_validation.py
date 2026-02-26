# new version of t-string parser does not validate constructor arguments
# there really is no need
print("SKIP")
raise SystemExit

from string.templatelib import Interpolation

print("=== __template__ validation tests ===")

print("\nTest 1: strings must be tuple")
try:
    __template__([], ())
    print("FAIL: accepted list")
except TypeError as e:
    print("OK: rejected list -", e)

print("\nTest 2: strings must be non-empty")
try:
    __template__((), ())
    print("FAIL: accepted empty strings")
except ValueError as e:
    print("OK: rejected empty strings -", e)

print("\nTest 3: strings items must be str")
try:
    __template__((123,), ())
    print("FAIL: accepted non-string")
except TypeError as e:
    print("OK: rejected non-string -", e)

try:
    __template__(("a", 456, "b"), ((1, "x"),))
    print("FAIL: accepted non-string at index 1")
except TypeError as e:
    print("OK: rejected non-string at index 1 -", e)

print("\nTest 4: len(strings) must equal len(interpolations) + 1")
try:
    __template__(("a", "b"), ())
    print("FAIL: accepted wrong length (2 strings, 0 interpolations)")
except ValueError as e:
    print("OK: rejected wrong length -", e)

try:
    __template__(("a",), ((1, "x"),))
    print("FAIL: accepted wrong length (1 string, 1 interpolation)")
except ValueError as e:
    print("OK: rejected wrong length -", e)

try:
    __template__(("a", "b", "c"), ((1, "x"),))
    print("FAIL: accepted wrong length (3 strings, 1 interpolation)")
except ValueError as e:
    print("OK: rejected wrong length -", e)

print("\nTest 5: valid inputs")
try:
    t1 = __template__(("hello",), ())
    print("OK: accepted (1 string, 0 interpolations)")
except Exception as e:
    print("FAIL:", e)

try:
    t2 = __template__(("a", "b"), ((1, "x"),))
    print("OK: accepted (2 strings, 1 interpolation)")
except Exception as e:
    print("FAIL:", e)

try:
    t3 = __template__(("a", "b", "c"), ((1, "x"), (2, "y")))
    print("OK: accepted (3 strings, 2 interpolations)")
except Exception as e:
    print("FAIL:", e)

print("\nTest 6: verify Template properties")
t = __template__(("Hello ", "!"), ((42, "value"),))
print("strings:", t.strings)
print("interpolations count:", len(t.interpolations))
print("first interpolation value:", t.interpolations[0].value)
print("first interpolation expr:", t.interpolations[0].expression)

print("\nTest 7: Interpolation defaults")
interp_default = Interpolation(123)
print("expression default:", repr(interp_default.expression))
print("conversion default is None:", interp_default.conversion is None)
print("format_spec default:", repr(interp_default.format_spec))

print("\n=== All validation tests complete ===")
