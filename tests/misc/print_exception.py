try:
    import uio as io
except ImportError:
    import io
import sys
if hasattr(sys, 'print_exception'):
    print_exception = sys.print_exception
else:
    import traceback
    print_exception = lambda e, f: traceback.print_exception(None, e, sys.exc_info()[2], file=f)

def print_exc(e):
    buf = io.StringIO()
    print_exception(e, buf)
    s = buf.getvalue()
    for l in s.split("\n"):
        # uPy on pyboard prints <stdin> as file, so remove filename.
        if l.startswith("  File "):
            l = l.split('"')
            print(l[0], l[2])
        # uPy and CPy tracebacks differ in that CPy prints a source line for
        # each traceback entry. In this case, we know that offending line
        # has 4-space indent, so filter it out.
        elif not l.startswith("    "):
            print(l)

# basic exception message
try:
    1/0
except Exception as e:
    print('caught')
    print_exc(e)

# exception message with more than 1 source-code line
def f():
    g()
def g():
    2/0
try:
    f()
except Exception as e:
    print('caught')
    print_exc(e)
