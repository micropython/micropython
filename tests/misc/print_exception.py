# Test sys.print_exception (MicroPython) / traceback.print_exception (CPython).

try:
    import io
    import sys
except ImportError:
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
        # MPy on pyboard prints <stdin> as file, so remove filename.
        if l.startswith("  File "):
            l = l.split('"')
            print(l[0], l[2])
        # MPy and CPy tracebacks differ in that CPy prints a source line for
        # each traceback entry. In this case, we know that offending line
        # has 4-space indent, so filter it out.
        elif not l.startswith("    "):
            print(l)


# basic exception message
try:
    raise Exception("msg")
except Exception as e:
    print("caught")
    print_exc(e)


# exception message with more than 1 source-code line
def f():
    g()


def g():
    raise Exception("fail")


try:
    f()
except Exception as e:
    print("caught")
    print_exc(e)

# Test that an exception propagated through a finally doesn't have a traceback added there
try:
    try:
        f()
    finally:
        print("finally")
except Exception as e:
    print("caught")
    print_exc(e)

# Test that re-raising an exception doesn't add traceback info
try:
    try:
        f()
    except Exception as e:
        print("reraise")
        print_exc(e)
        raise e
except Exception as e:
    print("caught")
    print_exc(e)


# Here we have a function with lots of bytecode generated for a single source-line, and
# there is an error right at the end of the bytecode.  It should report the correct line.
def f():
    f([1, 2], [1, 2], [1, 2], {1: 1, 1: 1, 1: 1, 1: 1, 1: 1, 1: 1, 1: f.X})
    return 1


try:
    f()
except Exception as e:
    print_exc(e)

# Test non-stream object passed as output object, only valid for MicroPython
if hasattr(sys, "print_exception"):
    try:
        sys.print_exception(Exception, 1)
        had_exception = False
    except OSError:
        had_exception = True
    assert had_exception
