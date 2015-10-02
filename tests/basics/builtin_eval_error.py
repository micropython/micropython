# test if eval raises SyntaxError

try:
    print(eval("[1,,]"))
except SyntaxError:
    print("SyntaxError")
