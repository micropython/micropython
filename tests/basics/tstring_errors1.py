try:
    from string.templatelib import Template, Interpolation
except ImportError:
    print("SKIP")
    raise SystemExit

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

print("\n=== Deep nesting test ===")
try:
    nested = "1" + " + 1" * 150
    code = f't"{{{nested}}}"'
    exec(code)
    print("Deep nesting: OK")
except Exception as e:
    print(f"Deep nesting error: {type(e).__name__}")

print("\n=== Trailing whitespace in expression ===")
try:
    x = 42
    code = 't"{x   }"'
    result = eval(code)
    print(f"Trailing whitespace: OK")
except Exception as e:
    print(f"Trailing whitespace error: {e}")

print("\n=== Single closing brace ===")
try:
    exec('t"test }"')
    print("ERROR: Single } should have raised SyntaxError")
except SyntaxError as e:
    print(f"Single }}: SyntaxError - {e}")

print("\n=== Escape in string within interpolation ===")

try:
    compile('x = 1; t"{x=:"', '<test>', 'exec')
except SyntaxError:
    print('Debug unclosed colon: SyntaxError')

try:
    compile('x = 1; t"{x=!"', '<test>', 'exec')
except SyntaxError:
    print('Debug unclosed exclaim: SyntaxError')

try:
    compile('t"prefix{incomplete"', '<test>', 'exec')
except SyntaxError as e:
    print('Unclosed brace literal:', e)

try:
    import sys
    if hasattr(sys.implementation, '_mpy'):
        compile('f"{x!invalid}"', '<test>', 'exec')
except SyntaxError:
    print('Malformed f-string: SyntaxError')

# NOTE: Error messages are shortened in MicroPython to avoid stack overflow
# during ROM compression on constrained platforms (Windows x86, ASan).
# CPython 3.14 message: "Template.__new__ *args need to be of type 'str' or 'Interpolation', got int"
# MicroPython message: "Template.__new__ args must be str or Interpolation, got 'int'"

try:
    t = Template("string", 123)
except TypeError as e:
    print('Type error int:', e)

try:
    t = Template("a", "b", 3.14)
except TypeError:
    print('Type error float: TypeError')

try:
    compile('t"text { more text"', '<test>', 'exec')
except SyntaxError as e:
    print('Unmatched brace:', e)

try:
    compile('x=1; t"{x= {nested}}"', '<test>', 'exec')
except SyntaxError:
    print('Nested debug braces: SyntaxError')

try:
    compile('t"{x!}"', '<test>', 'exec')
except SyntaxError:
    print('Missing conversion: SyntaxError')

try:
    compile('t"{x!z}"', '<test>', 'exec')
except SyntaxError as e:
    print('Invalid conversion:', e)

try:
    compile('t"{x!r' + chr(0x0b) + ':10}"', '<test>', 'exec')
except SyntaxError as e:
    print('Vertical tab:', e)

try:
    compile('t"{x!r@:10}"', '<test>', 'exec')
except SyntaxError as e:
    print('Invalid char after conversion:', e)

try:
    x = 'test'
    exec('result = t"{x!r :10}"')
    print('Space after conversion: OK')
except Exception as e:
    print(f'Unexpected error: {e}')

try:
    x = 'test'
    exec('result = t"{x!s\\t:10}"')
    print('Tab after conversion: OK')
except Exception as e:
    print(f'Unexpected error: {e}')

try:
    x = 'test'
    exec('result = t"{x!a\\n:10}"')
    print('Newline after conversion: OK')
except Exception as e:
    print(f'Unexpected error: {e}')

try:
    x = 'test'
    exec('result = t"{x!r\\r:10}"')
    print('CR after conversion: OK')
except Exception as e:
    print(f'Unexpected error: {e}')

try:
    x = 'test'
    exec('result = t"{x!s\\f:10}"')
    print('Form-feed after conversion: OK')
except Exception as e:
    print(f'Unexpected error: {e}')

print("\n=== Mixed prefixes ===")
for src in ('ft"{x}"', 'tf"{x}"', 'frt"{x}"', 'rtf"{x}"', 'trf"{x}"'):
    try:
        exec(src)
        print(src.split('"')[0] + ": OK (BUG!)")
    except SyntaxError as e:
        prefix = src.split('"')[0]
        # Check if we get the specific error message or generic one
        if "'f' and 't' prefixes are incompatible" in str(e):
            print(f"{prefix}: incompatible prefixes")
        else:
            print(f"{prefix}: invalid syntax")
