import sys

try:
    sys.settrace
except AttributeError:
    print("SKIP")
    raise SystemExit


def trace_tick_handler(frame, event, arg):
    print("FRAME", frame)
    print("LASTI", frame.f_lasti)
    return None


def f():
    x = 3
    return x


sys.settrace(trace_tick_handler)
f()
sys.settrace(None)
