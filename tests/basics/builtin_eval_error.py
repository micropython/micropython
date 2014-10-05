# test if eval raises SyntaxError

try:
    print(eval("[1, *a]"))
except SyntaxError:
    print("SyntaxError")
