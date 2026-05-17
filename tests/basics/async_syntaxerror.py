# test syntax errors using async

try:
    exec
except NameError:
    print("SKIP")
    raise SystemExit


def test_syntax(code):
    try:
        exec(code)
        print("no SyntaxError")
    except SyntaxError:
        print("SyntaxError")


test_syntax("async for x in (): x")
test_syntax("async with x: x")
