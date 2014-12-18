import _io as io # uPy does not have io module builtin
import sys
if hasattr(sys, 'print_exception'):
    print_exception = sys.print_exception
else:
    import traceback
    print_exception = lambda e, f: traceback.print_exception(None, e, None, file=f)

try:
    XXX
except Exception as e:
    print('caught')
    buf = io.StringIO()
    print_exception(e, buf)
    s = buf.getvalue()
    for l in s.split("\n"):
        # uPy on pyboard prints <stdin> as file, so remove filename.
        if l.startswith("  File "):
            print(l[:8], l[-23:])
        # uPy and CPy tracebacks differ in that CPy prints a source line for
        # each traceback entry. In this case, we know that offending line
        # has 4-space indent, so filter it out.
        elif not l.startswith("    "):
            print(l)
