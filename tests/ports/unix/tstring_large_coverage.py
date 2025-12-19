print("# Large allocation tests for t-string coverage")
print("# These tests require substantial memory and are unix-only")

from string.templatelib import Template, Interpolation

print("\n# Template constructor overflow")
try:
    interps = [Interpolation(0, "x")] * 4096
    strings = [""] * 4097
    t = Template(*strings, *interps)
    print("ERROR: Should have raised OverflowError")
except OverflowError:
    print("Template() overflow: OverflowError (correct)")

print("\n# Template varargs constructor n_strings overflow")
try:
    args = []
    for i in range(4095):
        args.append("")
        args.append(Interpolation(0, "x"))
    args.append("")
    t = Template(*args)
    print("ERROR: Should have raised OverflowError")
except OverflowError:
    print("Template varargs n_strings overflow: OverflowError (correct)")

print("\n# __template__ type validation (list instead of tuple)")
try:
    interps = [Interpolation(0, "x")] * 4096
    __template__([""], interps)
    print("ERROR: Should have raised TypeError")
except TypeError:
    print("__template__ list validation: TypeError (correct)")

print("\n# __template__ overflow")
try:
    interps = tuple((i, f"x{i}", None, "") for i in range(4096))
    __template__(("",), interps)
    print("ERROR: Should have raised OverflowError")
except OverflowError:
    print("__template__ overflow: OverflowError (correct)")

print("\n# Runtime overflow > 4095 interpolations")
try:
    code = 't"' + "{x}" * 4096 + '"'
    exec(f"x = 1; result = {code}")
    print("ERROR: Should have raised OverflowError")
except (OverflowError, SyntaxError, MemoryError) as e:
    print(f"4096 interpolations (runtime): {type(e).__name__} (correct)")

print("\n# Parse-time overflow > 4095 interpolations")
try:
    code = 't"' + "{x}" * 4096 + '"'
    compile(f"x = 1; result = {code}", "<test>", "exec")
    print("ERROR: Should have raised OverflowError")
except (OverflowError, SyntaxError, MemoryError) as e:
    print(f"4096 interpolations (parse-time): {type(e).__name__} (correct)")

print("\n# __template__ with maximum valid sizes")
try:
    strings = tuple(["s"] * 4095)
    interps_list = []
    for i in range(4094):
        interps_list.append((i, f"x{i}", None, ""))
    interps = tuple(interps_list)
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

print("\n# Too many segments")
try:
    code = 't"' + "x{y}" * 2200 + '"'
    exec(f"y = 1; result = {code}")
    print("ERROR: Should have raised SyntaxError/OverflowError")
except (SyntaxError, OverflowError, MemoryError) as e:
    print(f"Too many segments: {type(e).__name__} (correct)")

print("\n# Approaching max interpolations")
try:
    code = 't"' + "{x}" * 4090 + '"'
    exec(f"x = 1; result = {code}")
    print(f"4090 interpolations: OK")
except Exception as e:
    print(f"4090 interpolations error: {type(e).__name__}")

print("\n# Empty expression (whitespace only)")
try:
    exec('t"{ }"')
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print("Empty expr (space): SyntaxError (correct)")

try:
    exec('t"{\t}"')
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print("Empty expr (tab): SyntaxError (correct)")

try:
    exec('t"{\n}"')
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print("Empty expr (newline): SyntaxError (correct)")

print("\n# Lexer escape sequence: invalid then valid")
try:
    exec("t'\\U00200000\\x41'")
    print("ERROR: Should have raised SyntaxError")
except SyntaxError as e:
    print("Invalid escape sequence: SyntaxError (correct)")

print("\n=== Large coverage tests completed! ===")
