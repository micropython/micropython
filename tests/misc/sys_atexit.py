# test sys.atexit() function

import usys

try:
    usys.atexit
except AttributeError:
    print("SKIP")
    raise SystemExit

some_var = None


def do_at_exit():
    print("done at exit:", some_var)


usys.atexit(do_at_exit)

some_var = "ok"
print("done before exit")
