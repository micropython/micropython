# Test template string (t-string) operations with heap allocation failure

import micropython
from string.templatelib import Template, Interpolation

# Pre-create objects for tests
strings1 = ("x" * 100,) * 10
interps1 = tuple(Interpolation(i, "x" + str(i)) for i in range(9))
i1 = Interpolation(1, "1")
i2 = Interpolation(2, "2")

# Template creation
micropython.heap_lock()
try:
    Template(strings1, interps1)
    print("FAIL: Template creation")
except MemoryError:
    print("OK: Template creation")
micropython.heap_unlock()

# Multiple string concatenation
micropython.heap_lock()
try:
    Template("first", "second", "third", "fourth")
    print("FAIL: Multi string concat")
except MemoryError:
    print("OK: Multi string concat")
micropython.heap_unlock()

# Mixed constructor
micropython.heap_lock()
try:
    Template("a", i1, "b", i2, "c", "d", "e")
    print("FAIL: Mixed constructor")
except MemoryError:
    print("OK: Mixed constructor")
micropython.heap_unlock()

# Template.__str__()
t = t"Hello {42} world {99}"
micropython.heap_lock()
try:
    t.__str__()
    print("FAIL: Template.__str__()")
except MemoryError:
    print("OK: Template.__str__()")
micropython.heap_unlock()

# Template.values property
vals = list(range(10))
t_many = t"{vals[0]}{vals[1]}{vals[2]}{vals[3]}{vals[4]}"
micropython.heap_lock()
try:
    t_many.values
    print("FAIL: Template.values")
except MemoryError:
    print("OK: Template.values")
micropython.heap_unlock()

# Large values array
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

# Template concatenation
t1 = t"Hello"
t2 = t"World"
micropython.heap_lock()
try:
    t1 + t2
    print("FAIL: Template concatenation")
except MemoryError:
    print("OK: Template concatenation")
micropython.heap_unlock()

# Template iterator
t_iter = t"a{1}b{2}c"
micropython.heap_lock()
try:
    iter(t_iter)
    print("FAIL: Template iterator")
except MemoryError:
    print("OK: Template iterator")
micropython.heap_unlock()

# Iterator next
t_iter2 = t"a{1}b{2}c{3}d"
it = iter(t_iter2)
micropython.heap_lock()
try:
    list(it)
    print("FAIL: Iterator next")
except MemoryError:
    print("OK: Iterator next")
micropython.heap_unlock()

# __template__ builtin
strings2 = ("test",) * 5
interps2 = ((42, "x", None, ""),) * 4
micropython.heap_lock()
try:
    __template__(strings2, interps2)
    print("FAIL: __template__ builtin")
except MemoryError:
    print("OK: __template__ builtin")
micropython.heap_unlock()

# Format spec interpolation
width = 10
t_fmt = t"{42:{width}d}"
micropython.heap_lock()
try:
    t_fmt.__str__()
    print("FAIL: Format spec interpolation")
except MemoryError:
    print("OK: Format spec interpolation")
micropython.heap_unlock()

# Debug format
x = 42
t_debug = t"{x=}"
micropython.heap_lock()
try:
    t_debug.__str__()
    print("FAIL: Debug format")
except MemoryError:
    print("OK: Debug format")
micropython.heap_unlock()

# Conversion with format
obj = "test"
t_conv = t"{obj!r:>10}"
micropython.heap_lock()
try:
    t_conv.__str__()
    print("FAIL: Conversion + format")
except MemoryError:
    print("OK: Conversion + format")
micropython.heap_unlock()

# String conversion (s)
t_s = t"{'test'!s}"
micropython.heap_lock()
try:
    t_s.__str__()
    print("FAIL: s conversion")
except MemoryError:
    print("OK: s conversion")
micropython.heap_unlock()

# ASCII conversion (a)
t_a = t"{'test'!a}"
micropython.heap_lock()
try:
    t_a.__str__()
    print("FAIL: a conversion")
except MemoryError:
    print("OK: a conversion")
micropython.heap_unlock()

# Complex format spec
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

# Simple expression
x = 42
t_simple = t"{x}"
micropython.heap_lock()
try:
    t_simple.__str__()
    print("FAIL: Simple expression")
except MemoryError:
    print("OK: Simple expression")
micropython.heap_unlock()

# Interpolation creation
micropython.heap_lock()
try:
    Interpolation("value", "expr", "r", ".2f")
    print("FAIL: Interpolation creation")
except MemoryError:
    print("OK: Interpolation creation")
micropython.heap_unlock()

# Template repr
t = t"test"
micropython.heap_lock()
try:
    repr(t)
    print("FAIL: Template repr")
except MemoryError:
    print("OK: Template repr")
micropython.heap_unlock()

# Interpolation repr
i = Interpolation(42, "x")
micropython.heap_lock()
try:
    repr(i)
    print("FAIL: Interpolation repr")
except MemoryError:
    print("OK: Interpolation repr")
micropython.heap_unlock()

def exhaust_heap(limit):
    allocations = []
    try:
        for _ in range(limit):
            allocations.append("x" * 1024)
    except MemoryError:
        pass
    return allocations

def test_many_interpolations_heap():
    # Pre-create variables before exhausting heap
    x1, x2, x3, x4, x5, x6, x7, x8, x9 = 1, 2, 3, 4, 5, 6, 7, 8, 9
    micropython.heap_lock()
    try:
        t = t"{x1}{x2}{x3}{x4}{x5}{x6}{x7}{x8}{x9}"
        print("FAIL: Many interpolations heap test")
    except MemoryError:
        print("OK: Many interpolations heap test")
    micropython.heap_unlock()

def test_template_str_heap():
    t = t"x{1}x{2}x{3}x{4}x"
    micropython.heap_lock()
    try:
        s = t.__str__()
        print("FAIL: Template str heap test")
    except MemoryError:
        print("OK: Template str heap test")
    micropython.heap_unlock()

def test_template_iter_heap():
    t = t"a{1}b{2}c"
    micropython.heap_lock()
    try:
        parts = list(iter(t))
        print("FAIL: Template iter heap test")
    except MemoryError:
        print("OK: Template iter heap test")
    micropython.heap_unlock()

def test_template_concat_heap():
    t1 = t"first"
    t2 = t"second"
    micropython.heap_lock()
    try:
        t3 = t1 + t2
        print("FAIL: Template concat heap test")
    except MemoryError:
        print("OK: Template concat heap test")
    micropython.heap_unlock()

def test_format_spec_heap():
    width = 10
    t = t"{42:{width}d}"
    micropython.heap_lock()
    try:
        s = t.__str__()
        print("FAIL: Format spec heap test")
    except MemoryError:
        print("OK: Format spec heap test")
    micropython.heap_unlock()

def test_debug_format_heap():
    value = 42
    t = t"{value}"
    micropython.heap_lock()
    try:
        s = t.__str__()
        print("FAIL: Debug format heap test")
    except MemoryError:
        print("OK: Debug format heap test")
    micropython.heap_unlock()

print("\n=== Heap allocation failure tests ===")
test_many_interpolations_heap()
test_template_str_heap()
test_template_iter_heap()
test_template_concat_heap()
test_format_spec_heap()
test_debug_format_heap()

# Additional tests from coverage.c
print("\n=== Coverage.c heap tests ===")

# Test creating interpolation with heap locked (from coverage.c)
micropython.heap_lock()
try:
    i = Interpolation(42, "x", None, None)
    print("FAIL: Interpolation with heap locked")
except MemoryError:
    print("OK: Interpolation creation with heap locked")
micropython.heap_unlock()

# Test parsing expression with heap locked (from coverage.c)
micropython.heap_lock()
try:
    t = eval('t"{x + 1}"')
    print("FAIL: Parse with heap locked")
except Exception as e:
    if type(e).__name__ == "MemoryError":
        print("OK: Parse with heap locked")
    else:
        print(f"Parse with heap locked: {type(e).__name__}")
micropython.heap_unlock()

print("=== Tests completed ===")