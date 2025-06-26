# Syntax errors and error handling for t-strings
# This file combines error tests and import tests

print("=== Syntax errors ===")

# Invalid prefix combinations
try:
    exec('bt"test"')
    print("ERROR: bt prefix should fail")
except SyntaxError as e:
    print(f"SyntaxError: {e}")

try:
    exec('ft"test"')
    print("ERROR: ft prefix should fail") 
except SyntaxError as e:
    print(f"SyntaxError: {e}")

# Unclosed string
try:
    exec('t"unclosed')
    print("ERROR: Unclosed string should fail")
except SyntaxError as e:
    print(f"SyntaxError: {e}")

print("\n=== Template behavior ===")

# PEP 750: Plain str(t) should return repr, not rendered content
name = "World"
t = t"Hello {name}"
try:
    # Direct str() on template should give repr-like output
    result = str(t)
    print(f"PEP 750: str(t) returns default object repr")
except:
    print("ERROR: str(t) failed")

# Templates don't support ordering operations
t1 = t"a"
t2 = t"b"
try:
    result = t1 < t2
    print("ERROR: Templates should not support ordering")
except TypeError as e:
    print(f"TypeError: Templates do not support ordering")

print("\n=== Implicit concatenation restrictions ===")

# Cannot mix t-string and regular string
try:
    exec('t"hello" "world"')
    print("ERROR: Should not allow t-string + string literal")
except SyntaxError as e:
    print(f"SyntaxError: Cannot mix t-string and regular string")

try:
    exec('"hello" t"world"')
    print("ERROR: Should not allow string literal + t-string")
except SyntaxError as e:
    print(f"SyntaxError: Cannot mix regular string and t-string")

# Cannot mix in a chain
try:
    exec('t"a" "b" t"c"')
    print("ERROR: Should not allow mixed chain")
except SyntaxError as e:
    print(f"SyntaxError: Cannot mix t-string and regular string in chain")

print("\n=== Import location tests ===")

# Templates should NOT be in builtins
try:
    Template
    print("ERROR: Template should not be a builtin")
except NameError:
    print("ERROR: Should not be able to import Template from builtins")

try:
    Interpolation
    print("ERROR: Interpolation should not be a builtin")
except NameError:
    print("ERROR: Should not be able to import Interpolation from builtins")

# Correct import location
from string.templatelib import Template, Interpolation
print("OK: Imported Template and Interpolation from string.templatelib")

# Test creating with imported types
t_manual = Template(("Hello ", "!"), (Interpolation("World", "name"),))
print(f"Created template with imported types: {type(t_manual).__name__}")

# Verify types
print(f"Template type name: {Template.__name__}")
print(f"Interpolation type name: {Interpolation.__name__}")

# Test that t-string literals create the same types
name = "World"
t_literal = t"Hello {name}"
print(f"Literal template type: {type(t_literal).__name__}")
print(f"Is same Template type: {type(t_literal) is Template}")
print(f"Is same Interpolation type: {type(t_literal.interpolations[0]) is Interpolation}")

print("\n=== Type checking ===")

# isinstance checks
print(f"isinstance(t_literal, Template): {isinstance(t_literal, Template)}")
i = Interpolation(42, "x")
print(f"isinstance(i, Interpolation): {isinstance(i, Interpolation)}")

# Type equality
t_manual2 = Template(("test",), ())
print(f"Manual template type == Template: {type(t_manual2) == Template}")

# Test with non-Template
fake = object()
print(f"isinstance(fake, Template): {isinstance(fake, Template)}")

print("\n=== Module structure ===")

# Check module hierarchy
import string
print(f"string module type: {type(string)}")
print(f"Has templatelib: {hasattr(string, 'templatelib')}")

# Verify correct module path
import string.templatelib
print(f"string.templatelib.Template: {string.templatelib.Template is Template}")
print(f"string.templatelib.Interpolation: {string.templatelib.Interpolation is Interpolation}")

print("\n=== Expression parsing errors ===")

# Invalid syntax in expression
try:
    exec('t"{1 +++ 2}"')
    print("ERROR: Invalid syntax should have failed")
except SyntaxError as e:
    print(f"Invalid syntax caught: {e}")

# Unclosed parentheses
try:
    exec('t"{(1 + 2}"')
    print("ERROR: Unclosed paren should have failed")
except SyntaxError as e:
    print(f"Unclosed paren caught: {e}")

# Invalid tokens
try:
    exec('t"{@}"')
    print("ERROR: Invalid token should have failed")
except SyntaxError as e:
    print(f"Invalid token caught: {e}")

# Lambda not allowed
try:
    exec('t"{lambda x: x}"')
    print("Lambda allowed in t-string")
except SyntaxError as e:
    print(f"Lambda error: {e}")

print("\nSyntax tests completed!")