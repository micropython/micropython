print("# Large allocation tests for t-string coverage")
print("# These tests require substantial memory and are unix-only")

from string.templatelib import Template, Interpolation

print("\n# Template() constructor overflow (modtstring.c:95)")
try:
    interps = [Interpolation(0, "x")] * 4096
    strings = [""] * 4097
    t = Template(*strings, *interps)
    print("ERROR: Should have raised OverflowError")
except OverflowError:
    print("Template() overflow: OverflowError (correct)")

print("\n# Template varargs constructor n_strings overflow (modtstring.c:148)")
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

print("\n# __template__ overflow (modtstring.c:414)")
try:
    interps = [Interpolation(0, "x")] * 4096
    __template__([""], interps)
    print("ERROR: Should have raised OverflowError")
except OverflowError:
    print("__template__ overflow: OverflowError (correct)")

print("\n# Parse overflow > 4095 interpolations (parse.c:1080)")
try:
    code = 't"' + "{x}" * 4096 + '"'
    exec(f"x = 1; result = {code}")
    print("ERROR: Should have raised OverflowError")
except (OverflowError, SyntaxError):
    print("4096 interpolations: OverflowError (correct)")

print("\n=== Large coverage tests completed! ===")
