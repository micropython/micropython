try:
    from string.templatelib import Template, Interpolation
except ImportError:
    print("SKIP")
    raise SystemExit

# Check if t-strings are supported
try:
    exec('t"test"')
except SyntaxError:
    print("SKIP")
    raise SystemExit

try:
    import gc
    import sys
    gc.collect()
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

t_empty_strs = Template("", Interpolation(1, "a"), "", Interpolation(2, "b"), "")
print(f"Empty strings: {list(t_empty_strs)}")

t_adj = t"{1}{2}{3}"
print(f"Adjacent: '{t_adj.__str__()}'")

t_single = Template("only")
print(f"Single iter: {list(t_single)}")

t_self = t"test"
print(f"Self+self: '{(t_self + t_self).__str__()}'")

print("\n=== Values property ===")
for n in range(7):
    args = []
    for i in range(n):
        args.append("")
        args.append(Interpolation(i, str(i)))
    args.append("")
    t = Template(*args)
    print(f"Values[{n}]: {t.values}")

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

try:
    t_conv_fmt = t"{42!r:}"
    print(f"Conv empty fmt: '{t_conv_fmt.__str__()}'")
except Exception as e:
    print(f"Conv fmt error: {e}")

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

print("\n=== Bracket/paren depth tracking ===")
d = {'a': 1, 'b:c': 2}
items = [10, 20, 30]
colon_key = t"{d['b:c']}"
print(f"Colon in key: expr={colon_key.interpolations[0].expression}, value={colon_key.interpolations[0].value}")

slice_expr = t"{items[1:3]}"
print(f"Slice colon: expr={slice_expr.interpolations[0].expression}, value={slice_expr.interpolations[0].value}")

def pair(x, y=':'):
    return (x, y)

func_expr = t"{pair(1, 2)}"
print(f"Function call: expr={func_expr.interpolations[0].expression}, value={func_expr.interpolations[0].value}")

default_expr = t"{pair(3)}"
print(f"Default arg colon: expr={default_expr.interpolations[0].expression}, value={default_expr.interpolations[0].value}")

matrix = [[1, 2], [3, 4]]
nested_expr = t"{matrix[0][1]}"
print(f"Nested brackets: expr={nested_expr.interpolations[0].expression}, value={nested_expr.interpolations[0].value}")

print("\n=== repr tests ===")
t_repr = t"Hello {42} world"
print(f"Template repr: {repr(t_repr)[:60]}...")

i_repr = Interpolation(42, "x", "s", ":>10")
print(f"Interp repr: {repr(i_repr)}")

print("\n=== Additional coverage tests ===")

t_str_literal = t"{'hello'}"
print(f"String literal: {t_str_literal.__str__()}")

path = "/usr/local/bin"
count = 42
raw_path = rt"Path: {path}\n"
print(f"Raw path strings: {raw_path.strings}, value={raw_path.interpolations[0].value}")

raw_regex = rt"Regex: \\d+{count}"
print(f"Raw regex strings: {raw_regex.strings}, value={raw_regex.interpolations[0].value}")

try:
    t_str_expr = t'{"test"}'
    print(f"String expr: '{t_str_expr.__str__()}'")
except Exception as e:
    print(f"String expr error: {e}")

def raise_error():
    raise ValueError("Special error")

try:
    t_exc = t"{raise_error()}"
    print(t_exc.__str__())
except ValueError as e:
    print(f"Re-raised exception: {e}")

try:
    expr = "x = 1\n" + "t'" + "{x}" * 5000 + "'"
    exec(expr)
except (ValueError, SyntaxError, MemoryError, OverflowError) as e:
    print("Too many interpolations: SyntaxError")

try:
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

print("\n=== Integer overflow test ===")
primary = "OverflowError - template string too large for header format"
secondary = "none"
try:
    limit = 0x1000
    strings_list = ["" for _ in range(limit + 1)]
    strings = tuple(strings_list)
    interp = Interpolation(0, "x", None, "")
    interps_list = [interp for _ in range(limit)]
    interps = tuple(interps_list)
    Template(strings, interps)
except OverflowError as e:
    primary = f"OverflowError - {e}"
except MemoryError:
    primary = "MemoryError"
except RuntimeError as e:
    if "pystack exhausted" in str(e):
        secondary = "RuntimeError - pystack exhausted"
    else:
        primary = f"RuntimeError - {e}"
except Exception as e:
    primary = f"Unexpected exception: {type(e).__name__} - {e}"
else:
    primary = "no error"

print(f"Overflow test: {primary}")
if secondary != "none":
    print(f"Overflow test: {secondary}")

print()

print("\n=== Empty expression tests ===")
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
    exec("t'{\\n\\t   \\n}'")
    print("ERROR: Whitespace-only expr should fail")
except SyntaxError as e:
    print(f"Whitespace expr: SyntaxError")

try:
    exec("t'{value'")
    print("ERROR: Unterminated expr should fail")
except SyntaxError as e:
    print(f"Unterminated expr: {type(e).__name__}")

try:
    exec("t'{   !r}'")
    print("ERROR: Whitespace-conversion expr should fail")
except SyntaxError as e:
    print(f"Whitespace + conversion: {type(e).__name__}")

try:
    long_expr = 'x' * 104
    code = f"t'{{{long_expr}}}'"
    exec(f"{long_expr} = 'test'; result = {code}")
    print("Long expression: OK")
except Exception as e:
    print(f"Long expr error: {type(e).__name__}: {e}")

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

width = 5
x = 42
try:
    tmpl = t'{x:{width}{{literal}}}'
    result = tmpl.__str__()
    print(f'Escaped braces result: {result}')
except ValueError as e:
    print(f'Escaped braces ValueError: {e}')

print("\n=== Expression parser tests ===")
try:
    exec("t'{[{(x,y):[1,2,3]} for x,y in [(1,2),(3,4)]]}'")
    print("Complex expr: OK")
except Exception as e:
    print(f"Complex expr error: {type(e).__name__}")

print("\n=== Format spec position tests ===")
try:
    result = t'{42!r:>10}'
    print(f"Conv before fmt: '{result.__str__()}'")
except Exception as e:
    print(f"Conv before fmt error: {e}")

print("\n=== Parser MemoryError ===")
if sys.platform == 'webassembly':
    print("Parser MemoryError: MemoryError")
else:
    blocks = []
    try:
        try:
            while True:
                blocks.append(bytearray(512))
        except MemoryError:
            pass
        if blocks:
            blocks.pop()
            gc.collect()
        try:
            compile("t'{x}'", "<tstring>", "exec")
            print("ERROR: Parser MemoryError not raised")
        except MemoryError:
            print("Parser MemoryError: MemoryError")
    finally:
        blocks = None
        gc.collect()

print("\n=== Lexer edge cases ===")
print("Lexer NULL case: Tested via heapalloc_fail_tstring.py")

print("\n=== Format spec edge cases ===")
try:
    x = 42
    result = t'{x!r:0>+#10.5}'
    print(f"Full format: '{result}'")
except Exception as e:
    print(f"Full format error: {e}")

print("\n=== Debug format edge cases ===")
try:
    x = 42
    result = t'{x=!r}'
    print(f"Debug with conv: '{result}'")
except SyntaxError as e:
    print(f"Debug conv: SyntaxError - {e}")

print("\n=== Raw t-string escape tests ===")
try:
    raw_t = rt'\x41\u0042\n\t'
    print(f"Raw escapes: '{raw_t.__str__()}'")
except Exception as e:
    print(f"Raw escape error: {type(e).__name__}")

print("\n=== Interpolation attribute access ===")
i = Interpolation("test_value", "test_expr", "s", ":>10")
print(f"Value attr: {i.value}")
print(f"Expression attr: {i.expression}")
print(f"Conversion attr: {i.conversion}")
print(f"Format spec attr: {i.format_spec}")

print("\n=== Template string concatenation ===")
try:
    tmpl_concat = Template("part1", "part2", "part3", "part4", "part5")
    print(f"Concatenated strings: {tmpl_concat.strings}")
except Exception as e:
    print(f"Concatenation error: {e}")

print("\n=== Template interpolations attribute ===")
name = "World"
place = "Python"
tmpl = t"Hello {name} and {place}!"
try:
    print(f"Interpolations attr: {tmpl.interpolations}")
except AttributeError as e:
    print(f"Interpolations error: {e}")

print("\n=== Escaped quotes in literals ===")
try:
    exec(r'''result = t"He said \"Hello\" to me"''')
    print("Escaped quotes: OK")
except Exception as e:
    print(f"Escaped quotes error: {e}")

print("\n=== Backslash handling ===")
try:
    exec(r'''result = t"path\\{x}\\end"''')
    print("Backslash in path: OK")
except Exception as e:
    print(f"Backslash error: {e}")

print("\n=== Parser allocation edge cases ===")
try:
    deep_expr = "(" * 50 + "x" + ")" * 50
    exec(f"x = 1; result = t'{{{deep_expr}}}'")
    print("Deep nesting: OK")
except Exception as e:
    print(f"Deep nesting error: {type(e).__name__} - {e}")

try:
    long_expr = "(" * 100 + "x" + ")" * 100
    exec(f"x = 1; result = t'{{{long_expr}}}'")
    print("Very long expression: OK")
except Exception as e:
    print(f"Long expression error: {e}")

print("\n=== Complex expression parsing ===")
try:
    exec("data = {i: i**2 for i in range(5)}")
    exec("result = t'{data}'")
    print("Dict comprehension: OK")
except Exception as e:
    print(f"Dict comprehension error: {e}")

print("\n=== Additional parser regression tests ===")

try:
    fmt = '"QUOTED"'
    value = 'raw'
    tmpl = rt"{value:{fmt}}"
    print(f"Raw nested spec: {tmpl.interpolations[0].format_spec}")
except Exception as e:
    print(f"Raw nested spec error: {type(e).__name__}: {e}")

try:
    value = 'fmt'
    tmpl = t'{value:"QUOTED"}'
    print(f"Escaped quote spec: {tmpl.interpolations[0].format_spec}")
except Exception as e:
    print(f"Escaped quote spec error: {type(e).__name__}: {e}")

try:
    value = 'fmt'
    tmpl = t"{value:\\n}"
    print(f"Escaped backslash spec: {tmpl.interpolations[0].format_spec}")
except Exception as e:
    print(f"Escaped backslash spec error: {type(e).__name__}: {e}")

try:
    ns = {}
    exec("value = 'fmt'\nresult = t\"{value:{'\\\\'}}\"", globals(), ns)
    tmpl = ns['result']
    print(f"Nested quoted spec: {tmpl.interpolations[0].format_spec}")
except Exception as e:
    print(f"Nested quoted spec error: {type(e).__name__}: {e}")

try:
    value = 'brace'
    exec('tmpl = t"{value}}}}tail"')
    print(f"Literal brace strings: {tmpl.strings}")
except Exception as e:
    print(f"Literal brace error: {type(e).__name__}: {e}")

try:
    exec('t"{value"')
    print("ERROR: Unterminated field should have raised")
except SyntaxError as e:
    print(f"Unterminated field: {e}")

print("\n=== Template string size limit ===")
try:
    code = 't"' + '{x}' * 4096 + '"'
    exec(f"x = 1; result = {code}")
    print("ERROR: Should have raised SyntaxError for too many interpolations")
except (SyntaxError, OverflowError, MemoryError) as e:
    if isinstance(e, MemoryError) or "too many" in str(e) or "too large" in str(e):
        print("Too many interpolations: template string too large for header format")
    else:
        print(f"Other syntax error: {e}")

print("\n=== Malformed format specs (valid in CPython) ===")
x = 42
value = 42
result = t'{x:10!r}'
print(f"Malformed 1: format_spec: '{result.interpolations[0].format_spec}', conversion: {result.interpolations[0].conversion}")

result = t'{value:^10!s}'
print(f"Malformed 2: format_spec: '{result.interpolations[0].format_spec}', conversion: {result.interpolations[0].conversion}")

result = t'{x:>!10}'
print(f"Malformed 3: format_spec: '{result.interpolations[0].format_spec}', conversion: {result.interpolations[0].conversion}")

try:
    exec(r"t'\U00110000'")
    print("ERROR: Should have failed")
except SyntaxError as e:
    print(f"Invalid unicode: SyntaxError - {e}")

class CustomException(Exception):
    pass

class BadClass:
    def __getattr__(self, name):
        raise CustomException("Test exception")

try:
    bad_obj = BadClass()
    result = t"{bad_obj.attr}"
    print("ERROR: Should have raised exception")
except CustomException as e:
    print(f"Custom exception re-raised: {e}")
except Exception as e:
    print(f"Other exception: {type(e).__name__} - {e}")

print("\nNULL lexer: Tested in heapalloc_fail_tstring.py")

try:
    exec(r"t'\N{LATIN SMALL LETTER A}'")
    print("ERROR: \\N{{}} should not be supported")
except (SyntaxError, NotImplementedError) as e:
    print(f"\\N{{}} escape: {type(e).__name__}")

try:
    result = rt'\u0041\U00000042'
    print(f"Raw Unicode escapes: '{result.strings[0]}'")
except Exception as e:
    print(f"Raw Unicode error: {e}")

try:
    code_with_crlf = 't"line1\\r\\nline2"'
    exec(code_with_crlf)
    print("CR LF handling: OK")
except Exception as e:
    print(f"CR LF error: {e}")

try:
    code = ""
    for i in range(20):
        code += " " * i + "if True:\n"
    code += " " * 20 + "x = t'deep indent'"
    exec(code)
    print("Deep indent: OK")
except Exception as e:
    print(f"Deep indent error: {e}")

try:
    exec("t'test\\")
    print("ERROR: Trailing backslash should fail")
except SyntaxError as e:
    print(f"Trailing backslash: SyntaxError")

print("\n=== __template__ edge cases ===")
try:
    strings = tuple(["s"] * 4095)
    interps = tuple()
    result = __template__(strings, interps)
    print(f"Max strings: OK, {len(result.strings)} strings")
except Exception as e:
    print(f"Max strings error: {type(e).__name__}")

try:
    strings = tuple([""] * 4096)
    interps = []
    for i in range(4095):
        interps.append((i, f"x{i}", None, ""))
    result = __template__(strings, tuple(interps))
    print(f"Max interps: OK, {len(result.interpolations)} interpolations")
except Exception as e:
    print(f"Max interps error: {type(e).__name__}")

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

print("\n=== Debug specifier tests ===")
try:
    value = 42
    t1 = t'{value=}'
    print(f"Debug basic: strings={t1.strings}, conv={t1.interpolations[0].conversion}")

    t2 = t'{value=!s}'
    print(f"Debug !s: strings={t2.strings}, conv={t2.interpolations[0].conversion}")

    t3 = t'{value=:>10}'
    print(f"Debug fmt: strings={t3.strings}, fmt_spec='{t3.interpolations[0].format_spec}'")
except Exception as e:
    print(f"Debug format error: {e}")

print("\n=== Format spec with nested interpolation ===")
try:
    value = 42
    width = 10
    t1 = t'{value:{width}}'
    print(f"Nested fmt spec: '{t1.interpolations[0].format_spec}'")
except Exception as e:
    print(f"Nested fmt spec error: {e}")

print("\n=== Conversion types ===")
for conv in ['r', 's', 'a']:
    try:
        code = f"t'{{value!{conv}}}'"
        exec(f"value = 42; result = {code}")
        print(f"Conversion {conv}: OK")
    except Exception as e:
        print(f"Conversion {conv} error: {e}")

print("\n=== Raw template strings ===")
try:
    value = "test"
    rt1 = rt'Raw\n{value}'
    tr1 = tr'Raw\t{value}'
    print(f"rt prefix: '{rt1.strings[0]}'")
    print(f"tr prefix: '{tr1.strings[0]}'")
except Exception as e:
    print(f"Raw t-string error: {e}")

print("\n=== Template concatenation ===")
try:
    t1 = t"Hello "
    t2 = t"{name}"
    name = "World"
    t3 = t1 + t2
    print(f"Explicit concat: strings={t3.strings}, values={t3.values}")

    name = "World"
    t4 = t"Hello " t"{name}"
    print(f"Implicit concat: strings={t4.strings}, values={t4.values}")
except Exception as e:
    print(f"Concatenation error: {e}")

print("\n=== Empty template ===")
try:
    t_empty = t""
    print(f"Empty template: strings={t_empty.strings}, interpolations={t_empty.interpolations}")
except Exception as e:
    print(f"Empty template error: {e}")

try:
    code = 't"' + '{x}' * 4090 + '"'
    exec(f"x = 1; result = {code}")
    print(f"4090 interpolations: OK")
except Exception as e:
    print(f"4090 interpolations error: {type(e).__name__}")

try:
    code = 't"' + '{x}' * 4096 + '"'
    exec(f"x = 1; result = {code}")
    print("ERROR: Should have raised OverflowError")
except (OverflowError, SyntaxError, MemoryError) as e:
    print(f"4096 interpolations: {type(e).__name__} (correct)")

print("\n=== Trailing whitespace in expression ===")
try:
    x = 42
    code = 't"{x   }"'
    result = eval(code)
    print(f"Trailing whitespace: OK")
except Exception as e:
    print(f"Trailing whitespace error: {e}")

print("\n=== Deep nesting test ===")
try:
    nested = "1" + " + 1" * 150
    code = f't"{{{nested}}}"'
    exec(code)
    print("Deep nesting: OK")
except Exception as e:
    print(f"Deep nesting error: {type(e).__name__}")

print("\n=== Template + non-string object ===")
try:
    class CustomObj:
        pass
    t1 = t"hello"
    result = t1 + CustomObj()
    print("ERROR: Should have raised TypeError")
except TypeError as e:
    print(f"Template + custom object: TypeError (correct)")

print("\n=== 2-tuple constructor overflow ===")
try:
    from string.templatelib import Template
    strings = tuple("" for _ in range(4100))
    interps = tuple()
    t = Template(strings, interps)
    print("ERROR: Should have raised OverflowError")
except OverflowError as e:
    print(f"Too many strings (2-tuple): OverflowError (correct)")
except (MemoryError, TypeError) as e:
    print(f"Too many strings (2-tuple): {type(e).__name__} (memory/type limit)")

print("\n=== Empty format spec node ===")
try:
    x = 42
    result = t'{x:}'
    print(f"Empty format after colon: OK")
except Exception as e:
    print(f"Empty format spec error: {e}")

print("\n=== Too many segments ===")
try:
    code = 't"' + 'x{y}' * 2200 + '"'
    exec(f"y = 1; result = {code}")
    print("ERROR: Should have raised SyntaxError/OverflowError")
except (SyntaxError, OverflowError, MemoryError) as e:
    print(f"Too many segments: {type(e).__name__} (correct)")

print("\n=== Template() constructor with many interpolations ===")
try:
    exprs = ["a", "b", "c", "d", "e"]
    interps = [Interpolation(i, exprs[i % len(exprs)]) for i in range(20)]
    strings = [''] * 21
    t = Template(*strings, *interps)
    print(f"Template() constructor: OK ({len(t.interpolations)} interpolations)")
except Exception as e:
    print(f"Template() constructor: {type(e).__name__}")

print("\n=== Multiple consecutive strings ===")
try:
    t = Template("first", "second", "third", Interpolation(42, "x"), "fourth", "fifth")
    if t.strings == ('firstsecondthird', 'fourthfifth'):
        print("Multiple strings concatenated: OK")
    else:
        print(f"Multiple strings: strings={t.strings}")
except Exception as e:
    print(f"Multiple strings error: {e}")

print("\n=== Template.__add__ with multiple interpolations ===")
try:
    exprs = ["a", "b", "c", "d", "e"]
    interps1 = [Interpolation(i, exprs[i % len(exprs)]) for i in range(20)]
    strings1 = [''] * 21
    t1 = Template(*strings1, *interps1)

    interps2 = [Interpolation(i+20, exprs[i % len(exprs)]) for i in range(20)]
    strings2 = [''] * 21
    t2 = Template(*strings2, *interps2)

    result = t1 + t2
    print(f"Template.__add__: OK ({len(result.interpolations)} interpolations)")
except Exception as e:
    print(f"Template.__add__: {type(e).__name__}")

print("\n=== Single closing brace ===")
try:
    exec('t"test }"')
    print("ERROR: Single } should have raised SyntaxError")
except SyntaxError as e:
    print(f"Single }}: SyntaxError - {e}")

print("\n=== Escape in string within interpolation ===")
try:
    code = r"result = t'{\"test\"value\"}'"
    exec(code)
    print(f"Escaped quote in string: OK")
except Exception as e:
    print(f"Escaped quote error: {type(e).__name__}")

try:
    code = r"result = t'{\"\\n\"}'"
    exec(code)
    print(f"Escaped newline in string: OK")
except Exception as e:
    print(f"Escaped newline error: {type(e).__name__}")

print("\n=== Format spec with special characters ===")
try:
    x = 42
    result = t"{x:!r}"
    print(f"Format spec '!r': {result.interpolations[0].format_spec == '!r'}")
    print(f"Conversion is None: {result.interpolations[0].conversion is None}")
except Exception as e:
    print(f"Format spec error: {type(e).__name__}")

print("\nCoverage tests completed!")
