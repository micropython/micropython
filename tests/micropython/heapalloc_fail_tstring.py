# Test template string (t-string) operations with heap allocation failure

import micropython
from string.templatelib import Template, Interpolation

# Template string allocation failures during various operations

def test(msg, func):
    micropython.heap_lock()
    try:
        func()
        print(f"FAIL: {msg} - expected MemoryError")
    except MemoryError:
        print(f"OK: {msg}")
    except Exception as e:
        print(f"FAIL: {msg} - got {type(e).__name__}: {e}")
    finally:
        micropython.heap_unlock()

# Test 1: Template creation with large strings
def test_template_creation():
    strings = ("x" * 1000,) * 10
    interps = tuple(Interpolation(i, f"x{i}") for i in range(9))
    Template(strings, interps)

test("Template creation", test_template_creation)

# Test 2: Template.__str__() with memory exhaustion
t = t"Hello {42} world {99}"
test("Template.__str__()", lambda: t.__str__())

# Test 3: Values property with large number of interpolations
vals = list(range(100))
t_many = t"{vals[0]}{vals[1]}{vals[2]}{vals[3]}{vals[4]}{vals[5]}{vals[6]}{vals[7]}{vals[8]}{vals[9]}"
test("Template.values heap allocation", lambda: t_many.values)

# Test 4: Template concatenation
t1 = t"Hello"
t2 = t"World"
test("Template concatenation", lambda: t1 + t2)

# Test 5: Iterator creation
t_iter = t"a{1}b{2}c"
test("Template iterator", lambda: iter(t_iter))

# Test 6: __template__ builtin with allocation failure
def test_builtin():
    strings = ("test",) * 10
    interps = ((42, "x", None, ""),) * 9
    __template__(strings, interps)

test("__template__ builtin", test_builtin)

# Test 7: Format spec evaluation with interpolations
# This tests the vstr allocations in format spec handling
width = 10
t_fmt = t"{42:{width}d}"
test("Format spec interpolation", lambda: t_fmt.__str__())

# Test 8: Debug format (=) handling
x = 42
t_debug = t"{x=}"
test("Debug format", lambda: t_debug.__str__())

# Test 9: Complex conversion handling  
obj = "test"
t_conv = t"{obj!r:>10}"
test("Conversion + format", lambda: t_conv.__str__())

# Test 10: Multiple format spec interpolations
fill = "*"
align = ">"
width = 10
precision = 2
t_complex = t"{3.14159:{fill}{align}{width}.{precision}f}"
test("Complex format spec", lambda: t_complex.__str__())

# Test 11: Interpolation object creation
test("Interpolation creation", lambda: Interpolation("value", "expr", "r", ".2f"))

# Test 12: Large values array allocation (heap path)
# Create a template with many interpolations to trigger heap allocation
n = 20
interps = tuple(Interpolation(i, f"x{i}") for i in range(n))
strings = ("",) * (n + 1)
t_large = Template(strings, interps)
test("Large values array", lambda: t_large.values)

# Test 13: Template iterator with allocation failure during iteration
# Create iterator before locking heap, then test iteration
t_iter2 = t"a{1}b{2}c{3}d"
it = iter(t_iter2)
test("Iterator next", lambda: list(it))

# Test 14: Format spec with escaped braces
t_escaped = t"{42:{{}}}"
test("Escaped braces format", lambda: t_escaped.__str__())

# Test 15: Empty expression parsing edge case
# This might trigger allocation in the parser
t_empty_expr = t"{}"
test("Empty expression", lambda: t_empty_expr.__str__())

print("\nHeap allocation failure tests completed!")