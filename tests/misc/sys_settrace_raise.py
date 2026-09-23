# test that an exception raised by a sys.settrace callback propagates into the
# traced program, and that tracing can be re-established afterwards

import sys

try:
    sys.settrace
except AttributeError:
    print("SKIP")
    raise SystemExit


class Unwind(BaseException):
    pass


calls = 0
raise_at = None


def trace_handler(frame, event, arg):
    global calls
    calls += 1
    if raise_at is not None and calls >= raise_at:
        raise Unwind()
    return trace_handler


def work():
    total = 0
    for i in range(3):
        total += i
    return total


def traced(label):
    global calls
    calls = 0
    sys.settrace(trace_handler)
    try:
        result = work()
        print(label, "returned", result)
    except Unwind:
        print(label, "unwound")
    finally:
        sys.settrace(None)
    return calls


# A callback that does not raise traces the whole call.
raise_at = None
baseline = traced("baseline")
print("baseline traced:", baseline > 0)

# A callback that raises unwinds the traced program.
raise_at = 2
traced("raising")

# The raise unsets the trace function, as it does on CPython.
print("gettrace after raise:", sys.gettrace())

# ...and tracing is re-establishable: a stuck recursion guard inside the VM
# would leave the callback silently never invoked again.
raise_at = None
after = traced("after")
print("re-armed:", after == baseline)

# A BaseException subclass is not swallowed by a target's `except Exception`,
# which is what makes this usable as a debugger's unwind.
raise_at = 2
calls = 0


def swallowing():
    try:
        return work()
    except Exception as er:
        return "swallowed " + type(er).__name__


sys.settrace(trace_handler)
try:
    print("swallowing returned", swallowing())
except Unwind:
    print("swallowing unwound")
finally:
    sys.settrace(None)

# sys.settrace(None) called from inside a traced frame stops tracing that frame
# too, not just frames entered afterwards.
raise_at = None
calls = 0


def stops_tracing():
    sys.settrace(None)
    at_stop = calls
    work()
    return at_stop


sys.settrace(trace_handler)
at_stop = stops_tracing()
sys.settrace(None)
print("no further calls after settrace(None):", calls == at_stop)
