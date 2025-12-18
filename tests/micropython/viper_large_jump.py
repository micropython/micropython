COUNT = 600


try:
    code = """
@micropython.viper
def f() -> int:
    x = 0
    while x < 10:
"""
    for i in range(COUNT):
        code += "        x += 1\n"
    code += "    return x"
    exec(code)
except MemoryError:
    print("SKIP-TOO-LARGE")
    raise SystemExit


print(f())
