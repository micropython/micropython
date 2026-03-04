# NOTE: Error messages are shortened in MicroPython to avoid stack overflow
# during ROM compression on constrained platforms (Windows x86, ASan).
# CPython 3.14 message: "Template.__new__ *args need to be of type 'str' or 'Interpolation', got int"
# MicroPython message: "Template.__new__ args must be str or Interpolation, got 'int'"

from string.templatelib import Template

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
