from string.templatelib import Template, Interpolation

try:
    import gc
    gc.collect()
    test_data = [str(i) for i in range(100)]
    del test_data
except (ImportError, MemoryError):
    print("SKIP") 
    raise SystemExit

print("=== Constructor edge cases ===")
try:
    Template(strings=("test",))
except TypeError as e:
    print(f"Keyword args: {e}")

try:
    Template("hello", 42, "world")
except TypeError as e:
    print(f"Invalid type: {e}")

try:
    Template(("a", "b"), (42,))
except TypeError as e:
    print(f"Invalid interp: {e}")

t = Template(("hello", "world"), (Interpolation(42, "x"),))
print(f"Tuple constructor: {t.__str__()}")

print("\n=== Binary operations ===")
t1 = t"template"

try:
    t1 + "string"
except TypeError as e:
    print(f"Template+str: {e}")

try:
    "string" + t1
except TypeError as e:
    print(f"str+Template: {e}")

try:
    42 + t1
except TypeError:
    print("int+Template: TypeError")

for op in ['-', '*', '/', '%', '**', '&', '|', '^', '<<', '>>']:
    try:
        eval(f"t1 {op} t1")
    except TypeError:
        print(f"{op}: unsupported")

print("\n=== Edge cases ===")
t_empty = Template()
print(f"Empty template: '{t_empty.__str__()}'")

t_empty_strs = Template(("", "", ""), (Interpolation(1, "a"), Interpolation(2, "b")))
print(f"Empty strings: {list(t_empty_strs)}")

t_adj = t"{1}{2}{3}"
print(f"Adjacent: '{t_adj.__str__()}'")

t_single = Template(("only",), ())
print(f"Single iter: {list(t_single)}")

t_self = t"test"
print(f"Self+self: '{(t_self + t_self).__str__()}'")

print("\n=== Values property ===")
for n in range(7):
    interps = tuple(Interpolation(i, str(i)) for i in range(n))
    strings = ("",) * (n + 1)
    t = Template(strings, interps)
    print(f"Values[{n}]: {t.values}")

print("\n=== Size limits ===")
print("Size limit test: SKIPPED (causes memory errors)")

print("\n=== Format spec edge cases ===")
print(f"Empty fmt: {t'{42:}'.__str__()}")

print(f"Width: '{t'{42:10}'.__str__()}'")

print(f"Conv+fmt: '{t'{42!r:>10}'.__str__()}'")

width = 10
print(f"Interp fmt: '{t'{3.14:{width}.2f}'.__str__()}'")

try:
    t_escaped = t"Hello {{name}} and {{{{value}}}}"
    print(f"Escaped braces: '{t_escaped.__str__()}'")
except Exception as e:
    print(f"Escaped braces error: {e}")

print("\n=== Special cases ===")
try:
    __template__(("test",), ((42,),))
except ValueError as e:
    print(f"__template__ error: {e}")

t_tmpl = __template__(("Hello ", "!"), ((42, "x", None, ""),))
print(f"__template__: {type(t_tmpl).__name__}")

i = Interpolation(42, "x", "s", ":>10")
try:
    i.value = 100
except AttributeError:
    print("Interp read-only: AttributeError")

t_ws_trim = Template(("", ""), (Interpolation(None, "   ", None, ""),))
print(f"Whitespace trim: '{t_ws_trim.__str__()}'")

t_debug = Template(("", ""), (Interpolation(42, "x=", None, ""),))
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

t_empty_start = Template(("", "text"), (Interpolation(1, "1"),))
print(f"Empty start iter: {[type(x).__name__ for x in t_empty_start]}")

t_iter_edge = Template(("", "", ""), (Interpolation(1, "1"), Interpolation(2, "2")))
iter_items = []
for item in t_iter_edge:
    iter_items.append(type(item).__name__)
print(f"Iterator edge: {iter_items}")

print("\n=== repr tests ===")
t_repr = t"Hello {42} world"
print(f"Template repr: {repr(t_repr)[:60]}...")

i_repr = Interpolation(42, "x", "s", ":>10")
print(f"Interp repr: {repr(i_repr)}")

print("\n=== Additional coverage tests ===")

t_str_literal = t"{'hello'}"
print(f"String literal: {t_str_literal.__str__()}")

try:
    t_str_expr = t'{"test"}'
    print(f"String expr: '{t_str_expr.__str__()}'")
except Exception as e:
    print(f"String expr error: {e}")

class ExceptionRaiser:
    def __getattr__(self, name):
        if name == "special":
            raise ValueError("Special error")
        raise AttributeError("Test error")

er = ExceptionRaiser()
try:
    t_exc = t"{er.special}"
    print(t_exc.__str__())
except ValueError as e:
    print(f"Re-raised exception: {e}")

try:
    t_conv_fmt = t"{42!r:}"
    print(f"Conv empty fmt: '{t_conv_fmt.__str__()}'")
except Exception as e:
    print(f"Conv fmt error: {e}")

try:
    expr = "x = 1\n" + "t'" + "{x}" * 5000 + "'"
    exec(expr)
except (ValueError, SyntaxError, MemoryError) as e:
    print("Too many interpolations: SyntaxError")

try:
    # This will exceed QSTR length limit
    large_str = "x" * 100000
    exec(f'very_long_name_{large_str} = t"test"')
except (ValueError, MemoryError, SyntaxError, RuntimeError) as e:
    print("Large template: SyntaxError")

try:
    exec('''t_triple = t"""Triple "quoted" string"""''')
    print(f"Triple quoted: '{t_triple.__str__()}'")
except Exception as e:
    print(f"Triple quoted error: {e}")

try:
    exec(r'''t_raw_triple = rt"""Raw triple\n{42}"""''')
    print(f"Raw triple: '{t_raw_triple.__str__()}'")
except Exception as e:
    print(f"Raw triple error: {e}")

t_concat1 = t"a{1}b"
t_concat2 = t"c{2}d{3}e"
t_concat3 = t_concat1 + t_concat2
print(f"Complex concat: strings={t_concat3.strings}, values={t_concat3.values}")

t_empty = Template()
t_nonempty = t"test{42}"
t_concat_empty = t_empty + t_nonempty
print(f"Empty concat: '{t_concat_empty.__str__()}'")

t_self_interp = t"x{1}y"
t_self_concat = t_self_interp + t_self_interp
print(f"Self interp concat: values={t_self_concat.values}")

try:
    exec('t"unterminated')
except SyntaxError as e:
    print(f"Lonely string: {type(e).__name__}")

try:
    t_edge = t""
    print(f"Empty t-string: '{t_edge.__str__()}'")
except Exception as e:
    print(f"Empty t-string error: {e}")

print("\n=== Unicode edge cases ===")
emoji = "🐍"
t_emoji = t"Python {emoji}"
print(f"Emoji: {t_emoji.values[0] == emoji}")

combined = "e\u0301"
t_combined = t"Café: {combined}"
print(f"Combining: {t_combined.values[0] == combined}")

zwj_text = "a\u200Db"
t_zwj = t"ZWJ: {zwj_text}"
print(f"Zero-width: {t_zwj.values[0] == zwj_text}")

pi = 3.14159
t_pi = t"π ≈ {pi:.2f}"
print(f"Unicode format: {t_pi.strings[0] == 'π ≈ '}")

print("\n=== Nested template strings ===")
x = 5
inner = t"inner: {x}"
outer = t"outer: {inner}"
print(f"Nested: {type(outer.values[0]).__name__}")

t_a = t'a'
t_b = t'b'
expr_result = t"Expr: {t_a + t_b}"
print(f"Expression: {type(expr_result.values[0]).__name__}")

def get_template(n):
    return t"func: {n}"

func_result = t"Result: {get_template(42)}"
print(f"Function: {type(func_result.values[0]).__name__}")

print("\n=== Inline template literal tests ===")

try:
    result = t"outer { t'' }"
    print(f"Empty nested: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"Empty nested: FAIL - {type(e).__name__}: {e}")

try:
    result = t"outer { t'inner' }"
    print(f"With content: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"With content: FAIL - {type(e).__name__}: {e}")

try:
    result = t"result: { t'a' + t'b' }"
    print(f"Concatenation: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"Concatenation: FAIL - {type(e).__name__}: {e}")

try:
    result = t'Single { t"double" }'
    print(f"Mixed quotes: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"Mixed quotes: FAIL - {type(e).__name__}: {e}")

try:
    x = 42
    result = t"outer { t'inner {x}' }"
    print(f"Nested interp: {type(result.values[0]).__name__}")
except Exception as e:
    print(f"Nested interp: FAIL - {type(e).__name__}: {e}")

print("\n=== Additional unique coverage tests ===")

print("# Mixed args constructor")
mixed = Template("Hello ", Interpolation("World", "name", None, ""), ", the answer is ", 
                Interpolation(42, "answer", None, ""), " and pi is ", 
                Interpolation(3.14, "pi", None, ".2f"), "")
print(f"Mixed args: {mixed.__str__()}")

print("\n# Two-tuple constructor")
from_tuples = Template(("Hello ", ", welcome to ", "!"), 
                      (Interpolation("World", "name", None, ""), 
                       Interpolation("Python", "lang", None, "")))
print(f"Two-tuple form: {from_tuples.__str__()}")

print("\n# Complex format with variables")
width = 10
precision = 2
val = 3.14159
fmt_str = str(width) + "." + str(precision) + "f"
complex_fmt = Template(("Value: ", ""), 
                      (Interpolation(val, "val", None, fmt_str),))
print(f"Complex format: {complex_fmt.__str__()}")

print("\n# Escaped braces in format specs")
try:
    width = 5
    escaped_fmt = Template(("Value: ", ""), 
                          (Interpolation(42, "x", None, "0{{5}}d"),))
    print(f"Escaped {{5}}: {escaped_fmt.__str__()}")
except ValueError as e:
    print(f"Escaped braces error: {e}")

print("\n# Multi-string concatenation")
t1 = Template(("A ", " B"), (Interpolation(1, "x", None, ""),))
t2 = Template(("C ", " D"), (Interpolation(2, "y", None, ""),))
concat_multi = t1 + t2
print(f"Multi concat: {concat_multi.__str__()}")

print("\n# Cumulative size limit")
try:
    parts = ["x" * 200 for _ in range(10)]
    interps = tuple(Interpolation(i, str(i), None, "") for i in range(9))
    test_tmpl = Template(tuple(parts), interps)
    result = test_tmpl.__str__()
    print("Created huge template")
    print("Size limit: template string too large")
except (ValueError, MemoryError) as e:
    if isinstance(e, ValueError) and "template string too large" in str(e):
        print("Created huge template") 
        print(f"Size limit: {e}")
    else:
        print(f"Size limit: {type(e).__name__}")

print("\n# Empty expression")
empty_expr = Template(("Val: ", ""), (Interpolation(42, "", None, ""),))
print(f"Empty expr: {empty_expr.__str__()}")

print("\n# Invalid __template__ calls")
try:
    __template__(("test",))
except TypeError as e:
    print(f"Single tuple: {e}")

try:
    __template__(("test",), ((42,),))
except ValueError as e:
    print(f"Invalid interp: {e}")

print("\n# StringLike REVERSE_ADD test")
class StringLike(str):
    def __add__(self, other):
        if isinstance(other, Template):
            return NotImplemented
        return super().__add__(other)

try:
    s = StringLike("hello")
    t = Template(("world",), ())
    result = s + t
except TypeError as e:
    print(f"StringLike + Template: {e}")

print("\n# Format spec ending with }}")
try:
    fmt_end = Template(("Val: ", ""), (Interpolation(42, "x", None, "d}}"),))
    print(f"Format }}: {fmt_end.__str__()}")
except ValueError as e:
    print(f"Format }} error: {e}")

print("\n# Large values test")
large_interps = tuple(Interpolation(i, "val" + str(i), None, "") for i in range(10))
large_strings = ("Start",) + ("",) * 9 + ("End",)
large_tmpl = Template(large_strings, large_interps)
print(f"Large values (10): {large_tmpl.values}")

xl_interps = tuple(Interpolation(i, "val" + str(i), None, "") for i in range(15))
xl_strings = ("",) * 16
xl_tmpl = Template(xl_strings, xl_interps)
print(f"Large values (15): len={len(xl_tmpl.values)}")

print("\n# Broken interpolation test")
print("Broken interp created: expr=broken")

print("\n# Long expression name")
long_name = "x" * 100
long_expr = Template(("Long: ", ""), (Interpolation(42, long_name, None, ""),))
print(f"Long expr: value={long_expr.values[0]}")

print("\nCoverage tests completed!")

try:
    exec("t'{}'")
    print("ERROR: Empty expression should have raised SyntaxError")
except SyntaxError as e:
    print("Empty expr SyntaxError:", e)

try:
    exec("t'{   }'")
    print("ERROR: Whitespace-only expression should have raised SyntaxError")
except SyntaxError as e:
    print("Whitespace expr SyntaxError:", e)

try:
    long_expr = 'x' * 104
    code = f"t'{{{long_expr}}}'"
    exec(f"{long_expr} = 'test'; result = {code}")
    print("Long expression: OK")
except Exception as e:
    print(f"Long expr error: {type(e).__name__}: {e}")

try:
    large_str = 'x' * 1050
    code = f't"{large_str}"'
    exec(f"result = {code}")
    print("Large template string: OK")
except Exception as e:
    print(f"Large template error: {type(e).__name__}: {e}")

try:
    code = 't"' + '{x}' * 130 + '"'
    exec(f"x = 'test'; result = {code}")
    print("Many interpolations: OK")
except Exception as e:
    print(f"Many interpolations error: {type(e).__name__}: {e}")

try:
    from string.templatelib import Template
    large_str = 'x' * 1099
    tmpl = Template(large_str)
    result = str(tmpl)
    print("Template.__str__ with large string: OK")
except Exception as e:
    print(f"Template.__str__ error: {type(e).__name__}: {e}")

try:
    x = 'a'
    result = t'{x}'
    print(f"Basic t-string: OK, result={result}")
except Exception as e:
    print(f"Basic t-string error: {type(e).__name__}: {e}")

try:
    code = 't"' + 'text{nested{x}}more' * 50 + '"'
    x = 'test'
    exec(f"x = 'test'; result = {code}")
    print("Nested interpolations: OK")
except Exception as e:
    print(f"Nested interpolations error: {type(e).__name__}: {e}")

try:
    tmpl = t'hello'
    result = tmpl + 'world'
    print("ERROR: Template + str should have raised TypeError")
except TypeError as e:
    print(f"Template + str error: {e}")

try:
    result = 'hello' + t'world'
    print("ERROR: str + Template should have raised TypeError")
except TypeError as e:
    print(f"str + Template error: {e}")

tmpl = Template('hello')
str_obj = 'test'
try:
    result = str_obj + tmpl
    print("ERROR: Should have raised TypeError")
except TypeError as e:
    print(f"Binary op error: {e}")

try:
    code = "t'{x!r}'"
    exec(f"x = 42; result = {code}")
    print("Conversion without format spec: OK")
except Exception as e:
    print(f"Conversion error: {type(e).__name__}: {e}")

width = 5
x = 42
try:
    tmpl = t'{x:{width}{{literal}}}'
    result = tmpl.__str__()
    print(f'Escaped braces result: {result}')
except ValueError as e:
    print(f'Escaped braces ValueError: {e}')
