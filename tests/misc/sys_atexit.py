# test sys.atexit() function

import sys

try:
    sys.atexit
except AttributeError:
    print("SKIP")
    raise SystemExit

some_var = None


def do_at_exit():
    print("done at exit:", some_var)


@sys.atexit
def do_at_exit_2():
    print("done at exit last")


sys.atexit(do_at_exit)

some_var = "ok"
print("done before exit")
