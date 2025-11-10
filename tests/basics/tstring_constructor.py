try:
    from string.templatelib import Template, Interpolation
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    import gc
    import sys
    gc.collect()
except (ImportError, MemoryError):
    print("SKIP")
    raise SystemExit

print("=== Constructor basic usage ===")
t = Template("hello ", Interpolation(42, "x"), "world")
print(f"Template repr: {t.__str__()}")

t_varargs = Template("Hello ", Interpolation("World", "name"), "!")
print(f"Varargs constructor: strings={t_varargs.strings}, values={t_varargs.values}")

t_concat = Template("A", "B", Interpolation(1, "value"), "C", "D")
print(f"Varargs merged strings: {t_concat.strings}")

t_leading = Template(Interpolation(1, "x"), " tail")
print(f"Leading interpolation strings: {t_leading.strings}")

t_trailing = Template("head ", Interpolation(2, "y"))
print(f"Trailing interpolation strings: {t_trailing.strings}")

t_interps_only = Template(Interpolation(1, "x"), Interpolation(2, "y"))
print(f"Interpolation only strings: {t_interps_only.strings}")

print("\n=== Special cases ===")

i = Interpolation(42, "x", "s", ":>10")
try:
    i.value = 100
except AttributeError:
    print("Interp read-only: AttributeError")

t_ws_trim = Template("", Interpolation(None, "   ", None, ""), "")
print(f"Whitespace trim: '{t_ws_trim.__str__()}'")

t_debug = Template("", Interpolation(42, "x=", None, ""), "")
print(f"Debug =: {t_debug.__str__()}")

class Custom:
    def __repr__(self): return "CustomRepr"
    def __str__(self): return "CustomStr"

obj = Custom()
print(f"Custom !r: {t'{obj!r}'.__str__()}")
print(f"Custom !s: {t'{obj!s}'.__str__()}")

t_str = t"test"
str_method = t_str.__str__
print(f"__str__ bound: {str_method()}")

t_empty_start = Template("", Interpolation(1, "1"), "text")
print(f"Empty start iter: {[type(x).__name__ for x in t_empty_start]}")

t_iter_edge = Template("", Interpolation(1, "1"), "", Interpolation(2, "2"), "")
iter_items = []
for item in t_iter_edge:
    iter_items.append(type(item).__name__)
print(f"Iterator edge: {iter_items}")

print("\n=== Values property ===")
for n in range(7):
    args = []
    for i in range(n):
        args.append("")
        args.append(Interpolation(i, str(i)))
    args.append("")
    t = Template(*args)
    print(f"Values[{n}]: {t.values}")

print("\n=== Multiple consecutive strings ===")
try:
    t = Template("first", "second", "third", Interpolation(42, "x"), "fourth", "fifth")
    if t.strings == ('firstsecondthird', 'fourthfifth'):
        print("Multiple strings concatenated: OK")
    else:
        print(f"Multiple strings: strings={t.strings}")
except Exception as e:
    print(f"Multiple strings error: {e}")

print("\n=== Template() constructor with many interpolations ===")
try:
    exprs = ["a", "b", "c", "d", "e"]
    interps = [Interpolation(i, exprs[i % len(exprs)]) for i in range(20)]
    strings = [''] * 21
    t = Template(*strings, *interps)
    print(f"Template() constructor: OK ({len(t.interpolations)} interpolations)")
except Exception as e:
    print(f"Template() constructor: {type(e).__name__}")

print("\n=== vstr string concatenation ===")
try:
    t1 = Template("part1", "part2", "part3", "part4", Interpolation(1, "x"), "end")
    result = t1.__str__()
    print(f"vstr concat: '{result}'")
except Exception as e:
    print(f"vstr concat error: {e}")

print("\n=== High byte handling ===")
try:
    result = t'\x7F\x80\x81\xFE\xFF'
    first_str = result.strings[0]
    print(f"High bytes: len={len(first_str)}, first=0x{ord(first_str[0]):02x}, last=0x{ord(first_str[-1]):02x}")
except Exception as e:
    print(f"High bytes error: {e}")

try:
    result = t'\200\201\377'
    print(f"Octal high bytes: OK, len={len(result.strings[0])}")
except Exception as e:
    print(f"Octal high bytes error: {e}")

print("\nConstructor tests completed!")
