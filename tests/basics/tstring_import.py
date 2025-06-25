# Test import and type system for template strings (t-strings)

print("=== Import location tests ===")

# Test that Template/Interpolation cannot be imported from builtins
try:
    from builtins import Template
    print("ERROR: Should not be able to import Template from builtins")
except ImportError:
    print("ImportError: Cannot import Template from builtins")

try:
    from builtins import Interpolation
    print("ERROR: Should not be able to import Interpolation from builtins")
except ImportError:
    print("ImportError: Cannot import Interpolation from builtins")

# Test correct import location
try:
    from string.templatelib import Template, Interpolation
    print("OK: Imported Template and Interpolation from string.templatelib")
except ImportError as e:
    print(f"ERROR: Failed to import from string.templatelib: {e}")

# Verify imported types work correctly
from string.templatelib import Template, Interpolation

# Create instances using imported types
i = Interpolation("World", "name", None, "")
t = Template(("Hello ", "!"), (i,))
print(f"Created template with imported types: {type(t).__name__}")

# Verify type names
print(f"Template type name: {Template.__name__}")
print(f"Interpolation type name: {Interpolation.__name__}")

# Test that t-string literals create the same types
t_literal = t"Hello {name}"
name = "World"
print(f"Literal template type: {type(t_literal).__name__}")
print(f"Is same Template type: {type(t_literal) is Template}")
print(f"Is same Interpolation type: {type(t_literal.interpolations[0]) is Interpolation}")

print("\n=== Type checking ===")

# isinstance checks
print(f"isinstance(t_literal, Template): {isinstance(t_literal, Template)}")
print(f"isinstance(i, Interpolation): {isinstance(i, Interpolation)}")

# Type identity checks
manual_template = Template(("test",), ())
print(f"Manual template type == Template: {type(manual_template) == Template}")

# Subclass checks (should be False - these are not meant to be subclassed)
class FakeTemplate:
    pass

fake = FakeTemplate()
print(f"isinstance(fake, Template): {isinstance(fake, Template)}")

print("\n=== Module structure ===")

# Check that string is now a package
import string
print(f"string module type: {type(string)}")
print(f"Has templatelib: {hasattr(string, 'templatelib')}")

# Check templatelib module contents
import string.templatelib
expected_attrs = ['Template', 'Interpolation']
for attr in expected_attrs:
    print(f"string.templatelib.{attr}: {hasattr(string.templatelib, attr)}")

print("\nImport tests completed!")