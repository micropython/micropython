# make sure syntax error works corrects for bad const definition

try:
    exec("a = const(x)")
except SyntaxError:
    print("SyntaxError")
