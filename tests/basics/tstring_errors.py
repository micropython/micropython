# Test error cases for t-strings

print("=== Syntax errors ===")

# Test that t-strings cannot be combined with b-strings
try:
    eval('bt"test"')
except SyntaxError:
    print("SyntaxError: cannot combine 'b' and 't' prefixes")

# Test that t-strings cannot be combined with f-strings  
try:
    eval('ft"test"')
except SyntaxError:
    print("SyntaxError: cannot combine 'f' and 't' prefixes")

# Test unclosed t-string
try:
    eval('t"unclosed')
except SyntaxError:
    print("SyntaxError: unclosed string")

print("\n=== Template behavior ===")

# Test that str(Template) returns default repr (PEP 750)
t = t"test"
s = str(t)
if isinstance(s, str) and "Template" in s and "strings=" in s:
    print("PEP 750: str(t) returns default object repr")
else:
    print(f"FAIL: str(t) should return default object repr, got: {repr(s)}")

# Test that templates are not ordered
t1 = t"a"
t2 = t"b"
try:
    result = t1 < t2
except TypeError:
    print("TypeError: Templates do not support ordering")

print("\n=== Implicit concatenation restrictions ===")

# Test t-string + regular string
try:
    compile('t"hello" "world"', '<string>', 'eval')
except SyntaxError:
    print("SyntaxError: Cannot mix t-string and regular string")

# Test regular string + t-string  
try:
    compile('"hello" t"world"', '<string>', 'eval')
except SyntaxError:
    print("SyntaxError: Cannot mix regular string and t-string")

# Test multiple mixed strings
try:
    compile('t"a" t"b" "c"', '<string>', 'eval')
except SyntaxError:
    print("SyntaxError: Cannot mix t-string and regular string in chain")

print("\n=== Import errors ===")

# Test that Template/Interpolation cannot be instantiated directly from builtins
try:
    from builtins import Template
    print("ERROR: Should not be able to import Template from builtins")
except ImportError:
    print("ImportError: Cannot import Template from builtins")

# Correct import location
try:
    from string.templatelib import Template, Interpolation
    print("OK: Correct import from string.templatelib")
except ImportError:
    print("ERROR: Failed to import from string.templatelib")

print("\nError tests completed!")