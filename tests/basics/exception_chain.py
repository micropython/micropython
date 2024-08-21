# CIRCUITPY-CHANGE: exception chaining is supported

try:
    Exception().__cause__
except AttributeError:
    print("SKIP")
    raise SystemExit

def print_exc_info(e):
    print("exception", type(e), e.args)
    print("context", type(e.__context__), e.__suppress_context__)
    print("cause", type(e.__cause__))

try:
    try:
        1/0
    except Exception as inner:
        raise RuntimeError() from inner
except Exception as e:
    print_exc_info(e)
print()

try:
    try:
        1/0
    except Exception as inner:
        raise RuntimeError() from OSError()
except Exception as e:
    print_exc_info(e)
print()


try:
    try:
        1/0
    except Exception as inner:
        raise RuntimeError()
except Exception as e:
    print_exc_info(e)
print()

try:
    try:
        1/0
    except Exception as inner:
        raise RuntimeError() from None
except Exception as e:
    print_exc_info(e)

try:
    try:
        raise RuntimeError()
    except Exception as inner:
        1/0
except Exception as e:
    print_exc_info(e)
