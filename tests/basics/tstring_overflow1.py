# new version of t-string parser does not have these limitations
print("SKIP")
raise SystemExit

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

print("=== Template string size limit ===")
try:
    code = 't"' + '{x}' * 4096 + '"'
    exec(f"x = 1; result = {code}")
    print("ERROR: Should have raised SyntaxError for too many interpolations")
except (SyntaxError, OverflowError, MemoryError) as e:
    if isinstance(e, MemoryError) or "too many" in str(e) or "too large" in str(e):
        print("Too many interpolations: template string too large for header format")
    else:
        print(f"Other syntax error: {e}")

print("\n=== Too many segments ===")
try:
    code = 't"' + 'x{y}' * 2200 + '"'
    exec(f"y = 1; result = {code}")
    print("ERROR: Should have raised SyntaxError/OverflowError")
except (SyntaxError, OverflowError, MemoryError) as e:
    print(f"Too many segments: {type(e).__name__} (correct)")

print("\n=== Template() constructor with many interpolations ===")
