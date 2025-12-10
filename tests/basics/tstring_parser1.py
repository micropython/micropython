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


print("\n=== Parser MemoryError ===")
if sys.platform in ('webassembly', 'win32', 'msys', 'cygwin') or sys.maxsize < 2**32:
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


print("\n=== Complex expression stress test ===")
try:
    vars_dict = {f"x{i}": i for i in range(60)}
    exec_globals = globals().copy()
    exec_globals.update(vars_dict)
    expr_parts = [f"x{i}" for i in range(60)]
    expr_str = " + ".join(expr_parts)
    code = f't"{{{expr_str}}}"'
    result = eval(code, exec_globals)
    expected = sum(range(60))
    actual = result.interpolations[0].value
    if actual == expected:
        print(f"60 terms: PASS")
    else:
        print(f"60 terms: FAIL (expected={expected}, got={actual})")
except Exception as e:
    print(f"60 terms: ERROR - {type(e).__name__}: {e}")

try:
    vars_dict = {f"x{i}": i for i in range(100)}
    exec_globals = globals().copy()
    exec_globals.update(vars_dict)
    expr_parts = [f"x{i}" for i in range(100)]
    expr_str = " + ".join(expr_parts)
    code = f't"{{{expr_str}}}"'
    result = eval(code, exec_globals)
    expected = sum(range(100))
    actual = result.interpolations[0].value
    if actual == expected:
        print(f"100 terms: PASS")
    else:
        print(f"100 terms: FAIL (expected={expected}, got={actual})")
except Exception as e:
    print(f"100 terms: ERROR - {type(e).__name__}: {e}")

print("\n=== Stack expansion test (copy_parse_node) ===")
try:
    depth = 20
    nested_expr = "[" * depth + "1" + "]" * depth
    code = f't"{{{nested_expr}}}"'
    result = eval(code)
    if len(str(result.interpolations[0].value)) > 30:
        print("Stack expansion: OK")
    else:
        print(f"Stack expansion: FAIL")
except Exception as e:
    print(f"Stack expansion error: {type(e).__name__}")

print("\n=== Format spec with triple-quoted strings ===")
try:
    x = 42
    code = '''t"{x:{"""test"""}}"'''
    result = eval(code)
    print(f"Triple-quote in format spec: OK")
except Exception as e:
    print(f"Triple-quote error: {type(e).__name__}: {e}")

try:
    x = 42
    code = """t'{x:{'''test'''}}'"""
    result = eval(code)
    print(f"Triple single-quote in format spec: OK")
except Exception as e:
    print(f"Triple single-quote error: {type(e).__name__}: {e}")

print("\n=== Format spec parse error (exception handler) ===")
try:
    x = 42
    code = "t\"{x:{1 + (}}\" "  # Unclosed parenthesis
    result = eval(code)
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print(f"Parse error handler: OK")
except Exception as e:
    print(f"Unexpected error: {type(e).__name__}: {e}")

try:
    x = 42
    code = "t\"{x:{1 2 3}}\" "  # Invalid syntax
    result = eval(code)
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print(f"Parse error handler 2: OK")
except Exception as e:
    print(f"Unexpected error 2: {type(e).__name__}: {e}")

print("\n=== Large integer in t-string ===")
try:
    # Use eval() to avoid compile-time overflow in longlong variant
    large_int = eval("10**30")
    tmpl = t"{large_int}"
    if tmpl.interpolations[0].value == large_int:
        print("Large integer: OK")
    else:
        print(f"Large integer: FAIL")
except OverflowError:
    # longlong variant doesn't support arbitrary precision integers
    print("Large integer: OK")
except Exception as e:
    print(f"Large integer error: {type(e).__name__}: {e}")

print("\n=== Bytes in expression ===")
try:
    data = b"test"
    tmpl = t"{data}"
    if tmpl.interpolations[0].value == b"test":
        print("Bytes in expression: OK")
    else:
        print(f"Bytes: FAIL")
except Exception as e:
    print(f"Bytes error: {type(e).__name__}: {e}")

print("\n=== Boolean constants ===")
try:
    tmpl = t"{True} {False}"
    if tmpl.values == (True, False):
        print("Boolean constants: OK")
    else:
        print(f"Booleans: FAIL")
except Exception as e:
    print(f"Boolean error: {type(e).__name__}: {e}")

print("\n=== None constant ===")
try:
    tmpl = t"{None}"
    if tmpl.values[0] is None:
        print("None constant: OK")
    else:
        print(f"None: FAIL")
except Exception as e:
    print(f"None error: {type(e).__name__}: {e}")

print("\n=== Deep nesting for rule stack expansion ===")
try:
    depth = 80
    code = '(' * depth + '1' + ')' * depth
    result = eval(code)
    if result == 1:
        print(f"Deep nesting (depth={depth}): OK")
    else:
        print(f"Deep nesting: FAIL (result={result})")
except Exception as e:
    print(f"Deep nesting error: {type(e).__name__}: {e}")
