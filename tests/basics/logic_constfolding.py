# tests logical constant folding in parser

def f_true():
    print('f_true')
    return True

def f_false():
    print('f_false')
    return False

print(0 or False)
print(1 or foo)
print(f_false() or 1 or foo)
print(f_false() or 1 or f_true())

print(0 and foo)
print(1 and True)
print(f_true() and 0 and foo)
print(f_true() and 1 and f_false())

print(not 0)
print(not False)
print(not 1)
print(not True)
print(not not 0)
print(not not 1)
