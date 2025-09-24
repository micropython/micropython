# test sys.tracebacklimit

try:
    import sys
    import io
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    sys.tracebacklimit = 1000
except AttributeError:
    print("SKIP")
    raise SystemExit

if hasattr(sys, "print_exception"):
    print_exception = sys.print_exception
else:
    import traceback

    print_exception = lambda e, f: traceback.print_exception(None, e, sys.exc_info()[2], file=f)


def print_exc(e):
    buf = io.StringIO()
    print_exception(e, buf)
    s = buf.getvalue()
    for l in s.split("\n"):
        # Remove filename.
        if l.startswith("  File "):
            l = l.split('"')
            print(l[0], l[2])
        # MPy and CPy tracebacks differ in that CPy prints a source line for
        # each traceback entry.  In this case, we know that offending line
        # has 4-space indent, so filter it out.
        elif not l.startswith("    "):
            print(l)


def f0():
    raise ValueError("value")


def f1():
    f0()


def f2():
    f1()


def f3():
    f2()


def ftop():
    try:
        f3()
    except ValueError as er:
        print_exc(er)


ftop()

for limit in range(4, -2, -1):
    print("limit", limit)
    sys.tracebacklimit = limit
    ftop()


# test deleting the attribute
print(hasattr(sys, "tracebacklimit"))
del sys.tracebacklimit
print(hasattr(sys, "tracebacklimit"))
