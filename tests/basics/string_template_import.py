# Test importing from string.templatelib module

# Test basic import
try:
    from string.templatelib import Template, Interpolation
    print("Import successful")
except ImportError as e:
    print(f"Import failed: {e}")
    raise SystemExit

# Verify imported types
print(f"Template: {Template}")
print(f"Interpolation: {Interpolation}")

# Verify module names (if supported)
if hasattr(Template, '__module__'):
    print(f"Template module: {Template.__module__}")
    print(f"Interpolation module: {Interpolation.__module__}")
else:
    print("__module__ attribute not supported in MicroPython")

# Test that imported types work with t-strings
t = t"Hello {42}"
print(f"t-string type: {type(t)}")
print(f"Is Template: {isinstance(t, Template)}")

interp = t.interpolations[0]
print(f"Interpolation type: {type(interp)}")
print(f"Is Interpolation: {isinstance(interp, Interpolation)}")

# Verify we can't instantiate these types directly
try:
    Template()
    print("ERROR: Should not be able to instantiate Template")
except TypeError as e:
    print(f"Cannot instantiate Template: {e}")

try:
    Interpolation()
    print("ERROR: Should not be able to instantiate Interpolation")
except TypeError as e:
    print(f"Cannot instantiate Interpolation: {e}")