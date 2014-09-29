# test comparison operators
@micropython.viper
def f(x:int, y:int):
    if x < y:
        print(x, "<", y)
    if x > y:
        print(x, ">", y)
    if x == y:
        print(x, "==", y)
    if x <= y:
        print(x, "<=", y)
    if x >= y:
        print(x, ">=", y)
    if x != y:
        print(x, "!=", y)

f(1, 1)
f(2, 1)
f(1, 2)
f(2, -1)
f(-2, 1)
