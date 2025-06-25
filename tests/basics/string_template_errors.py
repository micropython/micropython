# Test error cases for t-strings

# Test that t-strings cannot be combined with b-strings
try:
    # This should raise a SyntaxError
    eval('bt"test"')
except SyntaxError:
    print("SyntaxError: cannot combine 'b' and 't' prefixes")

# Test that t-strings cannot be combined with f-strings  
try:
    # This should raise a SyntaxError
    eval('ft"test"')
except SyntaxError:
    print("SyntaxError: cannot combine 'f' and 't' prefixes")

# Test unclosed t-string
try:
    eval('t"unclosed')
except SyntaxError:
    print("SyntaxError: unclosed string")

# Test invalid conversion specifier
try:
    x = 1
    # Only r, s, a are valid
    eval('t"{x!x}"')
except SyntaxError:
    print("SyntaxError: invalid conversion character")

# Test that Template objects don't have custom __str__ (PEP 750)
try:
    from string.templatelib import Template
except ImportError:
    # Fallback for direct access
    from builtins import Template
t = t"test"
# PEP 750: str(t) should return default object repr
s = str(t)
# Check that it's a string but not the formatted content
# MicroPython returns "Template(strings=(...), interpolations=(...))" format
if isinstance(s, str) and "Template" in s and "strings=" in s:
    print("PEP 750: str(t) returns default object repr")
else:
    print("FAIL: str(t) should return default object repr, got:", repr(s))

# Test that templates are not ordered
t1 = t"a"
t2 = t"b"
try:
    result = t1 < t2
except TypeError:
    print("TypeError: Templates do not support ordering")

# Test implicit concatenation mixing - t-string + regular string
try:
    compile('t"hello" "world"', '<string>', 'eval')
except SyntaxError:
    print("SyntaxError: Cannot mix t-string and regular string in implicit concatenation")

# Test implicit concatenation mixing - regular string + t-string  
try:
    compile('"hello" t"world"', '<string>', 'eval')
except SyntaxError:
    print("SyntaxError: Cannot mix regular string and t-string in implicit concatenation")

# Test implicit concatenation with multiple strings
try:
    compile('t"a" t"b" "c"', '<string>', 'eval')
except SyntaxError:
    print("SyntaxError: Cannot mix t-string and regular string in multiple concatenation")