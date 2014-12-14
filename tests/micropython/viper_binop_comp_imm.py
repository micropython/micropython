# comparisons with immediate boundary values
@micropython.viper
def f(a: int):
    print(a == -1, a == -255, a == -256, a == -257)

f(-1)
f(-255)
f(-256)
f(-257)
