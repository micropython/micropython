# builtin eval

try:
    eval
except NameError:
    print("SKIP")
    raise SystemExit

eval('1 + 2')
eval('1 + 2\n')
eval('1 + 2\n\n#comment\n')

x = 4
eval('x')

eval('lambda x: x + 10')(-5)

y = 6
eval('lambda: y * 2')()

# A trailing backslash at the end of the source is a line continuation which
# continues past the end of the input.  This must raise SyntaxError: it
# previously silently evaluated to None, and tripped an assertion in builds
# with assertions enabled.
# https://github.com/micropython/micropython/issues/19061
for _src in ("123\\", "123\\\n", "1 + 2\\"):
    try:
        eval(_src)
    except SyntaxError:
        print("SyntaxError")

# a line continuation followed by more source still works
print(eval("1 + \\\n2"))
