# Advanced template string tests for edge cases and 100% code coverage

print("=== Type validation and constructors ===")

# Test Template constructor validation
from string.templatelib import Template, Interpolation

# Invalid strings type
try:
    Template("not a tuple", ())
except TypeError as e:
    print(f"strings validation: {e}")

# Invalid interpolations type
try:
    Template((), "not a tuple")
except TypeError as e:
    print(f"interpolations validation: {e}")

# Non-string in strings tuple
try:
    Template((42, "string"), ())
except TypeError as e:
    print(f"strings element validation: {e}")

# Non-Interpolation in interpolations tuple
try:
    Template(("string",), (42,))
except TypeError as e:
    print(f"interpolations element validation: {e}")

# Interpolation constructor validation
try:
    Interpolation()  # Missing args
except TypeError:
    print("Interpolation no args: TypeError")

try:
    Interpolation(1, 2, 3, 4, 5)  # Too many args
except TypeError:
    print("Interpolation too many args: TypeError")

print("\n=== Object representations ===")

# Template repr
t = t"test {42}"
print(f"Template repr: {repr(t)}")

# Interpolation repr
i = Interpolation(42, "x", "r", ".2f")
print(f"Interpolation repr: {repr(i)}")

# Interpolation attributes
print(f"value={i.value}, expr={i.expression}, conv={i.conversion}, fmt={i.format_spec}")

print("\n=== Memory and performance tests ===")

# Large number of interpolations (tests heap allocation)
vals = list(range(10))
t_many = t"{vals[0]}{vals[1]}{vals[2]}{vals[3]}{vals[4]}{vals[5]}{vals[6]}{vals[7]}{vals[8]}{vals[9]}"
print(f"Many interpolations: count={len(t_many.interpolations)}, values={t_many.values}")

# Very long string
long_str = "x" * 500
t_long = t"Start: {long_str} :End"
print(f"Long string length: {len(t_long.interpolations[0].value)}")

# Template size handling
try:
    big_str = "x" * 1000
    t_big = t"{big_str}{big_str}{big_str}{big_str}{big_str}"
    print(f"Large template created: {len(t_big.__str__())}")
except ValueError as e:
    print(f"Template size limit: {e}")

print("\n=== Iterator edge cases ===")

# Iterator with various patterns
t_pattern1 = t"{1}{2}{3}"  # Only interpolations
t_pattern2 = t"a{1}b{2}c"  # Alternating
t_pattern3 = t"start{1}{2}middle{3}{4}end"  # Mixed

for name, template in [("Only interp", t_pattern1), ("Alternating", t_pattern2), ("Mixed", t_pattern3)]:
    items = [type(x).__name__ for x in template]
    print(f"{name}: {items}")

# Iterator state machine
t_iter = t"a{1}b{2}c"
iter1 = iter(t_iter)
item1 = next(iter1)
item2 = next(iter1)
iter2 = iter(t_iter)  # New iterator
item3 = next(iter2)
print(f"Iterator restart: first={item1}, second from new={item3}")

# Template with all empty strings
t_empty_strings = Template(("", "", ""), (Interpolation(1, "a"), Interpolation(2, "b")))
items = list(t_empty_strings)
print(f"All empty strings iteration: {len(items)} items")

print("\n=== Concatenation edge cases ===")

# Concatenation with non-template types
t1 = t"test"
try:
    result = t1 + 42
except TypeError:
    print("OK: Cannot concatenate Template with int")

try:
    result = 42 + t1
except TypeError:
    print("OK: Cannot concatenate int with Template")

# Empty template concatenation
t_empty = t""
t_content = t"content"
result1 = t_empty + t_content
result2 = t_content + t_empty
print(f"Empty concat: {result1.strings} and {result2.strings}")

# Concatenation creating adjacent empty strings
t_end_interp = t"a{1}"
t_start_interp = t"{2}b"
t_concat = t_end_interp + t_start_interp
print(f"Adjacent interps concat: strings={t_concat.strings}")

print("\n=== Format spec interpolation edge cases ===")

# Format spec with only interpolation
precision = 2
t_fmt_only = t"{3.14159:{precision}}"
print(f"Format only interpolation: '{t_fmt_only.interpolations[0].format_spec}'")

# Multiple interpolations in format spec
width = 10
precision = 2
fill = '*'
value = 99.999
t_complex_fmt = t"{value:{fill}>{width}.{precision}f}"
result = t_complex_fmt.__str__()
print(f"Complex format result: '{result}'")

# Empty interpolation in format spec
empty = ""
t_empty_fmt = t"{42:{empty}d}"
print(f"Empty format result: '{t_empty_fmt.__str__()}'")

# Escaped braces in strings
t_escaped = t"{{value}}: {42}"
print(f"Escaped braces: strings={t_escaped.strings}")

print("\n=== Expression parser tests ===")

# Complex expressions
x, y, z = 10, 20, 30
t_math = t"Math: {x + y * z} {x ** 2} {x // y} {x % y}"
print(f"Complex math: {t_math.values}")

# Object operations
class TestObj:
    value = 42
    def method(self):
        return "result"

obj = TestObj()
t_obj = t"Attr: {obj.value} Method: {obj.method()}"
print(f"Object ops: {t_obj.values}")

# Container operations
lst = [1, 2, 3, 4, 5]
dct = {"key": "value"}
t_cont = t"List: {lst[1:3]} Dict: {dct.get('key', 'default')}"
print(f"Container ops: {t_cont.values}")

# Lambda for lazy evaluation
t_lambda = t"Lazy: {(lambda: 'evaluated')}"
print(f"Lambda callable: {callable(t_lambda.interpolations[0].value)}")
if callable(t_lambda.interpolations[0].value):
    print(f"Lambda result: {t_lambda.interpolations[0].value()}")

print("\n=== Attribute access tests ===")

# Template attributes are immutable
t_test = t"test {42}"
try:
    t_test.strings = ("modified",)
except AttributeError:
    print("OK: Template.strings is immutable")

# Interpolation attributes are immutable
i_test = t_test.interpolations[0]
try:
    i_test.value = 99
except AttributeError:
    print("OK: Interpolation.value is immutable")

# Non-existent attribute
try:
    _ = t_test.nonexistent
except AttributeError:
    print("OK: AttributeError for non-existent attribute")

print("\n=== Special format cases ===")

# All conversion types
value = 123
t_all_conv = t"{value} {value!r} {value!s} {value!a}"
result = t_all_conv.__str__()
print(f"All conversions length: {len(result)}")

# Format with conversions
t_fmt_conv = t"{42!r:>10} {42!s:<10} {42!a:^10}"
result = t_fmt_conv.__str__()
print(f"Format with conversions: '{result}'")

# Debug format variations
lst = [1, 2, 3]
t_debug_expr = t"{lst[0] + lst[1]=}"
print(f"Debug complex: '{t_debug_expr.strings[0]}', value={t_debug_expr.interpolations[0].value}")

# Special characters
t_special = t"Special: \x00\x01\x02\xff"
print(f"Special chars length: {len(t_special.strings[0])}")

print("\n=== Error handling ===")

# __template__ with invalid format
try:
    result = __template__(("test",), ((42,),))  # Missing fields
except ValueError:
    print("__template__ error: ValueError")

# Invalid syntax in expression (compile-time error)
try:
    compile('t"{@invalid}"', '<string>', 'eval')
except SyntaxError:
    print("OK: Invalid expression syntax")

# Invalid conversion (accepted in MicroPython)
i_invalid = Interpolation(42, "expr", "x", "")
print(f"Invalid conversion accepted: conv='{i_invalid.conversion}'")

print("\nAdvanced tests completed!")