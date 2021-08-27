# test atexit module

try:
    import atexit
except ImportError:
    print("SKIP")
    raise SystemExit


@atexit.register
def skip_at_exit():
    print("skip at exit")


@atexit.register
def do_at_exit(*args, **kwargs):
    print("done at exit:", args, kwargs)


atexit.unregister(skip_at_exit)
atexit.register(do_at_exit, "ok", 1, arg="2", param=(3, 4))
print("done before exit")
