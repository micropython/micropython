# Test template string (t-string) operations with heap allocation failure

import micropython
from string.templatelib import Template, Interpolation

# Pre-create objects that will be used in tests
strings1 = ("x" * 100,) * 10
interps1 = tuple(Interpolation(i, "x" + str(i)) for i in range(9))

# Test 1: Template creation with heap locked
micropython.heap_lock()
try:
    Template(strings1, interps1)
    print("FAIL: Template creation")
except MemoryError:
    print("OK: Template creation")
micropython.heap_unlock()

# Test 2: Template.__str__() with heap locked
t = t"Hello {42} world {99}"
micropython.heap_lock()
try:
    t.__str__()
    print("FAIL: Template.__str__()")
except MemoryError:
    print("OK: Template.__str__()")
micropython.heap_unlock()

# Test 3: Template.values property
vals = list(range(10))
t_many = t"{vals[0]}{vals[1]}{vals[2]}{vals[3]}{vals[4]}"
micropython.heap_lock()
try:
    t_many.values
    print("FAIL: Template.values heap allocation")
except MemoryError:
    print("OK: Template.values heap allocation")
micropython.heap_unlock()

# Test 4: Template concatenation
t1 = t"Hello"
t2 = t"World"
micropython.heap_lock()
try:
    t1 + t2
    print("FAIL: Template concatenation")
except MemoryError:
    print("OK: Template concatenation")
micropython.heap_unlock()

# Test 5: Template iterator
t_iter = t"a{1}b{2}c"
micropython.heap_lock()
try:
    iter(t_iter)
    print("FAIL: Template iterator")
except MemoryError:
    print("OK: Template iterator")
micropython.heap_unlock()

# Test 6: __template__ builtin
strings2 = ("test",) * 5
interps2 = ((42, "x", None, ""),) * 4
micropython.heap_lock()
try:
    __template__(strings2, interps2)
    print("FAIL: __template__ builtin")
except MemoryError:
    print("OK: __template__ builtin")
micropython.heap_unlock()

# Test 7: Format spec interpolation
width = 10
t_fmt = t"{42:{width}d}"
micropython.heap_lock()
try:
    t_fmt.__str__()
    print("FAIL: Format spec interpolation")
except MemoryError:
    print("OK: Format spec interpolation")
micropython.heap_unlock()

# Test 8: Debug format
x = 42
t_debug = t"{x=}"
micropython.heap_lock()
try:
    t_debug.__str__()
    print("FAIL: Debug format")
except MemoryError:
    print("OK: Debug format")
micropython.heap_unlock()

# Test 9: Conversion with format
obj = "test"
t_conv = t"{obj!r:>10}"
micropython.heap_lock()
try:
    t_conv.__str__()
    print("FAIL: Conversion + format")
except MemoryError:
    print("OK: Conversion + format")
micropython.heap_unlock()

# Test 10: Complex format spec
fill = "*"
align = ">"
width = 10
precision = 2
t_complex = t"{3.14159:{fill}{align}{width}.{precision}f}"
micropython.heap_lock()
try:
    t_complex.__str__()
    print("FAIL: Complex format spec")
except MemoryError:
    print("OK: Complex format spec")
micropython.heap_unlock()

# Test 11: Interpolation creation
micropython.heap_lock()
try:
    Interpolation("value", "expr", "r", ".2f")
    print("FAIL: Interpolation creation")
except MemoryError:
    print("OK: Interpolation creation")
micropython.heap_unlock()

# Test 12: Large values array
n = 20
interps3 = tuple(Interpolation(i, "x" + str(i)) for i in range(n))
strings3 = ("",) * (n + 1)
t_large = Template(strings3, interps3)
micropython.heap_lock()
try:
    t_large.values
    print("FAIL: Large values array")
except MemoryError:
    print("OK: Large values array")
micropython.heap_unlock()

# Test 13: Iterator next
t_iter2 = t"a{1}b{2}c{3}d"
it = iter(t_iter2)
micropython.heap_lock()
try:
    list(it)
    print("FAIL: Iterator next")
except MemoryError:
    print("OK: Iterator next")
micropython.heap_unlock()

# Test 14: Escaped braces format
t_escaped = t"{42:{{}}}"
micropython.heap_lock()
try:
    t_escaped.__str__()
    print("FAIL: Escaped braces format")
except MemoryError:
    print("OK: Escaped braces format")
micropython.heap_unlock()

# Test 15: Simple expression
x = 42
t_simple = t"{x}"
micropython.heap_lock()
try:
    t_simple.__str__()
    print("FAIL: Simple expression")
except MemoryError:
    print("OK: Simple expression")
micropython.heap_unlock()

print()
print("Heap allocation failure tests completed!", end="")