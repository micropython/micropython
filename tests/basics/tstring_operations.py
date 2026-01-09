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

print("\n=== Binary operations ===")
t1 = t"template"

try:
    t1 + "string"
except TypeError as e:
    print(f"Template+str: TypeError")

try:
    "string" + t1
except TypeError as e:
    print(f"str+Template: TypeError")

try:
    42 + t1
except TypeError:
    print("int+Template: TypeError")

for op in ['-', '*', '/', '%', '**', '&', '|', '^', '<<', '>>']:
    try:
        eval(f"t1 {op} t1")
    except TypeError:
        print(f"{op}: unsupported")

print("\n=== Template.__add__ with multiple interpolations ===")
try:
    exprs = ["a", "b", "c", "d", "e"]
    interps1 = [Interpolation(i, exprs[i % len(exprs)]) for i in range(20)]
    strings1 = [''] * 21
    t1 = Template(*strings1, *interps1)

    interps2 = [Interpolation(i+20, exprs[i % len(exprs)]) for i in range(20)]
    strings2 = [''] * 21
    t2 = Template(*strings2, *interps2)

    result = t1 + t2
    print(f"Template.__add__: OK ({len(result.interpolations)} interpolations)")
except Exception as e:
    print(f"Template.__add__: {type(e).__name__}")

print("\n=== Template + non-string object ===")
try:
    class CustomObj:
        pass
    t1 = t"hello"
    result = t1 + CustomObj()
    print("ERROR: Should have raised TypeError")
except TypeError as e:
    print(f"Template + custom object: TypeError (correct)")

print("\n=== 2-tuple constructor overflow ===")
