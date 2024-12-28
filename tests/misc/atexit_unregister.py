# test atexit.unregister is enabled.

try:
    import atexit

    atexit.unregister
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


def do_at_exit():
    print("should not done at exit")


atexit.register(do_at_exit)


@atexit.register
def do_at_exit_2():
    print("should be run")


atexit.register(do_at_exit)

atexit.unregister(do_at_exit)

print("done before exit")
