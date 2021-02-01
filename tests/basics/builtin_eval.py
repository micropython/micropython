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
