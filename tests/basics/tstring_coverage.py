# Comprehensive coverage tests for t-strings
from string.templatelib import Template, Interpolation

print("=== Constructor edge cases ===")
# Keyword argument error
try:
    Template(strings=("test",))
except TypeError as e:
    print(f"Keyword args: {e}")

# Invalid type error
try:
    Template("hello", 42, "world")
except TypeError as e:
    print(f"Invalid type: {e}")

# Invalid interpolation in tuple
try:
    Template(("a", "b"), (42,))
except TypeError as e:
    print(f"Invalid interp: {e}")

# Direct tuple constructor
t = Template(("hello", "world"), (Interpolation(42, "x"),))
print(f"Tuple constructor: {t.__str__()}")

print("\n=== Binary operations ===")
t1 = t"template"

# Template + str (error)
try:
    t1 + "string"
except TypeError as e:
    print(f"Template+str: {e}")

# str + Template (error)
try:
    "string" + t1
except TypeError as e:
    print(f"str+Template: {e}")

# Non-string + Template
try:
    42 + t1
except TypeError:
    print("int+Template: TypeError")

# Unsupported operations
for op in ['-', '*', '/', '%', '**', '&', '|', '^', '<<', '>>']:
    try:
        eval(f"t1 {op} t1")
    except TypeError:
        print(f"{op}: unsupported")

print("\n=== Edge cases ===")
# Empty template
t_empty = Template()
print(f"Empty template: '{t_empty.__str__()}'")

# Empty strings with interpolations
t_empty_strs = Template(("", "", ""), (Interpolation(1, "a"), Interpolation(2, "b")))
print(f"Empty strings: {list(t_empty_strs)}")

# Adjacent interpolations
t_adj = t"{1}{2}{3}"
print(f"Adjacent: '{t_adj.__str__()}'")

# Single element iteration
t_single = Template(("only",), ())
print(f"Single iter: {list(t_single)}")

# Self concatenation
t_self = t"test"
print(f"Self+self: '{(t_self + t_self).__str__()}'")

print("\n=== Values property ===")
# Test with 0-6 interpolations (covering stack and heap paths)
for n in range(7):
    interps = tuple(Interpolation(i, str(i)) for i in range(n))
    strings = ("",) * (n + 1)
    t = Template(strings, interps)
    print(f"Values[{n}]: {t.values}")

print("\n=== Size limits ===")
# Skip size limit test - causes memory allocation errors
print("Size limit test: SKIPPED (causes memory errors)")

print("\n=== Format spec edge cases ===")
# Empty format spec
print(f"Empty fmt: {t'{42:}'.__str__()}")

# Width only
print(f"Width: '{t'{42:10}'.__str__()}'")

# Conversion with format
print(f"Conv+fmt: '{t'{42!r:>10}'.__str__()}'")

# Interpolated format spec
width = 10
print(f"Interp fmt: '{t'{3.14:{width}.2f}'.__str__()}'")

# Escaped braces
try:
    # Test double braces are converted to single braces
    t_escaped = t"Hello {{name}} and {{{{value}}}}"
    print(f"Escaped braces: '{t_escaped.__str__()}'")
except Exception as e:
    print(f"Escaped braces error: {e}")

print("\n=== Special cases ===")
# __template__ builtin error
try:
    __template__(("test",), ((42,),))
except ValueError as e:
    print(f"__template__ error: {e}")

# Valid __template__ call
t_tmpl = __template__(("Hello ", "!"), ((42, "x", None, ""),))
print(f"__template__: {type(t_tmpl).__name__}")

# Interpolation attributes (read-only)
i = Interpolation(42, "x", "s", ":>10")
try:
    i.value = 100
except AttributeError:
    print("Interp read-only: AttributeError")

# Whitespace trimming
t_ws_trim = Template(("", ""), (Interpolation(None, "   ", None, ""),))
print(f"Whitespace trim: '{t_ws_trim.__str__()}'")

# Debug format with =
t_debug = Template(("", ""), (Interpolation(42, "x=", None, ""),))
print(f"Debug =: {t_debug.__str__()}")

# Custom object conversions
class Custom:
    def __repr__(self): return "CustomRepr"
    def __str__(self): return "CustomStr"

obj = Custom()
print(f"Custom !r: {t'{obj!r}'.__str__()}")
print(f"Custom !s: {t'{obj!s}'.__str__()}")

# Template __str__ method access
t_str = t"test"
str_method = t_str.__str__
print(f"__str__ bound: {str_method()}")

# Iterator with empty string at start
t_empty_start = Template(("", "text"), (Interpolation(1, "1"),))
print(f"Empty start iter: {[type(x).__name__ for x in t_empty_start]}")

# Iterator edge case - multiple empty strings between interpolations
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

# Test string literal in template expression (MP_PARSE_NODE_STRING)
t_str_literal = t"{'hello'}"
print(f"String literal: {t_str_literal.__str__()}")

# Test expression parsing edge cases
# String expressions with quotes
try:
    t_str_expr = t'{"test"}'
    print(f"String expr: '{t_str_expr.__str__()}'")
except Exception as e:
    print(f"String expr error: {e}")

# Exception re-raising in expression parser
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

# Test format spec edge case (fmt_end = conversion_pos)
try:
    # Test conversion with empty format
    t_conv_fmt = t"{42!r:}"
    print(f"Conv empty fmt: '{t_conv_fmt.__str__()}'")
except Exception as e:
    print(f"Conv fmt error: {e}")

# Test too many interpolations
# Create a template string with many interpolations to hit the limit
try:
    # Try to create a very large number of interpolations
    # MicroPython has a limit on interpolations
    expr = "x = 1\n" + "t'" + "{x}" * 5000 + "'"
    exec(expr)
except (ValueError, SyntaxError, MemoryError) as e:
    print(f"Too many interpolations: {type(e).__name__}")

# Test template string size limit
try:
    # Create a very large template string
    large_str = "x" * 100000
    exec(f't"{large_str}"')
except (ValueError, MemoryError, RuntimeError) as e:
    print(f"Large template: {type(e).__name__}")

# Test lexer edge cases
# Triple quoted t-strings
try:
    exec('''t_triple = t"""Triple "quoted" string"""''')
    print(f"Triple quoted: '{t_triple.__str__()}'")
except Exception as e:
    print(f"Triple quoted error: {e}")

# Raw triple quoted
try:
    exec(r'''t_raw_triple = rt"""Raw triple\n{42}"""''')
    print(f"Raw triple: '{t_raw_triple.__str__()}'")
except Exception as e:
    print(f"Raw triple error: {e}")

# Test template concatenation specific paths
# Concatenating templates with different interpolation counts
t_concat1 = t"a{1}b"
t_concat2 = t"c{2}d{3}e"
t_concat3 = t_concat1 + t_concat2
print(f"Complex concat: strings={t_concat3.strings}, values={t_concat3.values}")

# Concatenate empty template with non-empty
t_empty = Template()
t_nonempty = t"test{42}"
t_concat_empty = t_empty + t_nonempty
print(f"Empty concat: '{t_concat_empty.__str__()}'")

# Self concatenation with interpolations
t_self_interp = t"x{1}y"
t_self_concat = t_self_interp + t_self_interp
print(f"Self interp concat: values={t_self_concat.values}")

# Test lonely string open (unterminated t-string)
try:
    exec('t"unterminated')
except SyntaxError as e:
    print(f"Lonely string: {type(e).__name__}")

# Test fstring lexer edge case (empty fstring_args)
try:
    # This might trigger the lex->chr0 path when fstring_args is empty
    t_edge = t""
    print(f"Empty t-string: '{t_edge.__str__()}'")
except Exception as e:
    print(f"Empty t-string error: {e}")

print("\n=== Unicode edge cases ===")
# Test Unicode characters in template strings
emoji = "🐍"
t_emoji = t"Python {emoji}"
print(f"Emoji: {t_emoji.values[0] == emoji}")

# Test combining characters
combined = "e\u0301"  # e with combining acute accent
t_combined = t"Café: {combined}"
print(f"Combining: {t_combined.values[0] == combined}")

# Test zero-width characters
zwj_text = "a\u200Db"  # Zero-width joiner
t_zwj = t"ZWJ: {zwj_text}"
print(f"Zero-width: {t_zwj.values[0] == zwj_text}")

# Test Unicode in format specs
pi = 3.14159
t_pi = t"π ≈ {pi:.2f}"
print(f"Unicode format: {t_pi.strings[0] == 'π ≈ '}")

print("\n=== Nested template strings ===")
# Test template string inside interpolation
x = 5
inner = t"inner: {x}"
outer = t"outer: {inner}"
print(f"Nested: {type(outer.values[0]).__name__}")

# Test template in expression
t_a = t'a'
t_b = t'b'
expr_result = t"Expr: {t_a + t_b}"
print(f"Expression: {type(expr_result.values[0]).__name__}")

# Test template in function
def get_template(n):
    return t"func: {n}"

func_result = t"Result: {get_template(42)}"
print(f"Function: {type(func_result.values[0]).__name__}")

print("\nCoverage tests completed!")
