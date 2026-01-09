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

print("\n=== Format spec edge cases ===")
try:
    x = 42
    result = t'{x!r:0>+#10.5}'
    print(f"Full format: '{result}'")
except Exception as e:
    print(f"Full format error: {e}")

print("\n=== Format spec with special characters ===")
try:
    x = 42
    result = t"{x:!r}"
    print(f"Format spec '!r': {result.interpolations[0].format_spec == '!r'}")
    print(f"Conversion is None: {result.interpolations[0].conversion is None}")
except Exception as e:
    print(f"Format spec error: {type(e).__name__}")

print("\nCoverage tests completed!")
print("\n=== Debug format edge cases ===")
try:
    x = 42
    result = t'{x=!r}'
    print(f"Debug with conv: '{result}'")
except SyntaxError as e:
    print(f"Debug conv: SyntaxError - {e}")


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
    print(f"\\N{{}} escape: SyntaxError")

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

print("\n=== Empty format spec node ===")
try:
    x = 42
    result = t'{x:}'
    print(f"Empty format after colon: OK")
except Exception as e:
    print(f"Empty format spec error: {e}")


print("\n=== Format spec with special characters (coverage) ===")
try:
    align = "<"
    width = 5
    value = "test"
    result = t'{value:{"^"*width}}'
    print(f"Double quotes expr: format_spec={repr(result.interpolations[0].format_spec)}")
except Exception as e:
    print(f"Double quotes expr error: {type(e).__name__}: {e}")

try:
    align = "<"
    width = 5
    value = "test"
    result = rt"{value:{align}{width}\\}"
    print(f"Backslash raw: format_spec={repr(result.interpolations[0].format_spec)}")
except Exception as e:
    print(f"Backslash raw error: {type(e).__name__}: {e}")

try:
    align = "<"
    width = 5
    value = "test"
    result = t"{value:{align}{width}\\}"
    print(f"Backslash non-raw: format_spec={repr(result.interpolations[0].format_spec)}")
except Exception as e:
    print(f"Backslash non-raw error: {type(e).__name__}: {e}")

try:
    align = "<"
    width = 5
    value = "test"
    result = t"{value:{align}{width}\n\t\r}"
    print(f"Multiple escapes: format_spec={repr(result.interpolations[0].format_spec)}")
except Exception as e:
    print(f"Multiple escapes error: {type(e).__name__}: {e}")
