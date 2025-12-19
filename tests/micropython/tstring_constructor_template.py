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

print("=== __template__ special cases ===")
try:
    __template__(("test",), ((42,),))
except ValueError as e:
    print(f"__template__ error: {e}")

result = __template__(("Hello ", "!"), ((42, "x", None, ""),))
print(f"__template__: {type(result).__name__}")
