# test atexit module

try:
    import atexit
except ImportError:
    print("SKIP")
    raise SystemExit

some_var = None


@atexit.register
def do_at_exit_1():
    print("atexit: registered first")


def do_at_exit_2():
    global some_var
    print("atexit: registered last with access to", some_var)


atexit.register(do_at_exit_2)

some_var = "local var"
print("printed before exit")

# Trigger micropython shutdown/soft-reset now.
raise SystemExit
