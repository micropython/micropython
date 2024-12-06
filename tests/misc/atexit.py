# test atexit module

try:
    import atexit
except ImportError:
    print("SKIP")
    raise SystemExit

some_var = None


def do_at_exit():
    print("done at exit:", some_var)


@atexit.register
def do_at_exit_2():
    print("done at exit last")


atexit.register(do_at_exit)

some_var = "ok"
print("done before exit")
