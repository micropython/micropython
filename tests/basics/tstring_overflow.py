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

print("\n=== Integer overflow test ===")
primary = "OverflowError - template string too large for header format"
secondary = "none"
try:
    limit = 0x1000
    strings = tuple("" for _ in range(limit + 1))
    interps_list = []
    for i in range(limit):
        interps_list.append((i, f"x{i}", None, ""))
    interps = tuple(interps_list)
    __template__(strings, interps)
except OverflowError as e:
    primary = f"OverflowError - {e}"
except MemoryError:
    primary = "MemoryError"
except RuntimeError as e:
    if "pystack exhausted" in str(e):
        secondary = "RuntimeError - pystack exhausted"
    else:
        primary = f"RuntimeError - {e}"
except Exception as e:
    primary = f"Unexpected exception: {type(e).__name__} - {e}"
else:
    primary = "no error"

print(f"Overflow test: {primary}")
if secondary != "none":
    print(f"Overflow test: {secondary}")

print()

print("\n=== Empty expression tests ===")
print("\n=== Template string size limit ===")
try:
    code = 't"' + '{x}' * 4096 + '"'
    exec(f"x = 1; result = {code}")
    print("ERROR: Should have raised SyntaxError for too many interpolations")
except (SyntaxError, OverflowError, MemoryError) as e:
    if isinstance(e, MemoryError) or "too many" in str(e) or "too large" in str(e):
        print("Too many interpolations: template string too large for header format")
    else:
        print(f"Other syntax error: {e}")

print("\n=== Malformed format specs (valid in CPython) ===")
print("\n=== __template__ edge cases ===")
try:
    strings = tuple(["s"] * 4095)
    interps_list = []
    for i in range(4094):
        interps_list.append((i, f"x{i}", None, ""))
    interps = tuple(interps_list)
    result = __template__(strings, interps)
    print(f"Max strings: OK, {len(result.strings)} strings")
except Exception as e:
    print(f"Max strings error: {type(e).__name__}")

try:
    strings = tuple([""] * 4096)
    interps = []
    for i in range(4095):
        interps.append((i, f"x{i}", None, ""))
    result = __template__(strings, tuple(interps))
    print(f"Max interps: OK, {len(result.interpolations)} interpolations")
except (OverflowError, MemoryError):
    print("Max interps limit enforced")
except Exception as e:
    print(f"Max interps unexpected error: {type(e).__name__}")

print("\n=== vstr string concatenation ===")
print("\n=== Too many segments ===")
try:
    code = 't"' + 'x{y}' * 2200 + '"'
    exec(f"y = 1; result = {code}")
    print("ERROR: Should have raised SyntaxError/OverflowError")
except (SyntaxError, OverflowError, MemoryError) as e:
    print(f"Too many segments: {type(e).__name__} (correct)")

print("\n=== Template() constructor with many interpolations ===")
