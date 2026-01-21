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

try:
    import gc
    import sys
    gc.collect()
except (ImportError, MemoryError):
    print("SKIP")
    raise SystemExit

# NOTE: Error messages are shortened in MicroPython to avoid stack overflow
# during ROM compression on constrained platforms (Windows x86, ASan).
# CPython 3.14 message: "Template.__new__ *args need to be of type 'str' or 'Interpolation', got int"
# MicroPython message: "Template.__new__ args must be str or Interpolation, got 'int'"

print("=== Constructor error messages ===")
try:
    Template(strings=("test",))
except TypeError as e:
    print(f"Keyword args: {e}")

try:
    Template("hello", 42, "world")
except TypeError as e:
    print(f"Invalid type: {e}")

try:
    Template("a", 42, "b")
except TypeError as e:
    print(f"Invalid type in varargs: {e}")

print("\nConstructor error tests completed!")
