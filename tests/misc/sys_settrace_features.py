import sys

try:
    sys.settrace
except AttributeError:
    print("SKIP")
    raise SystemExit


def print_stacktrace(frame, level=0):
    # Ignore CPython specific helpers.
    if frame.f_globals["__name__"].find("importlib") != -1:
        print_stacktrace(frame.f_back, level)
        return

    print(
        "%2d: %s@%s:%s => %s:%d"
        % (
            level,
            "  ",
            frame.f_globals["__name__"],
            frame.f_code.co_name,
            # reduce full path to some pseudo-relative
            "misc" + "".join(frame.f_code.co_filename.split("tests/misc")[-1:]),
            frame.f_lineno,
        )
    )

    if frame.f_back:
        print_stacktrace(frame.f_back, level + 1)


class _Prof:
    trace_count = 0

    def trace_tick(self, frame, event, arg):
        self.trace_count += 1
        print_stacktrace(frame)


__prof__ = _Prof()

alice_handler_set = False


def trace_tick_handler_alice(frame, event, arg):
    print("### trace_handler::Alice event:", event)
    __prof__.trace_tick(frame, event, arg)
    return trace_tick_handler_alice


bob_handler_set = False


def trace_tick_handler_bob(frame, event, arg):
    print("### trace_handler::Bob event:", event)
    __prof__.trace_tick(frame, event, arg)
    return trace_tick_handler_bob


def trace_tick_handler(frame, event, arg):
    # Ignore CPython specific helpers.
    if frame.f_globals["__name__"].find("importlib") != -1:
        return

    print("### trace_handler::main event:", event)
    __prof__.trace_tick(frame, event, arg)

    if frame.f_code.co_name != "factorial":
        return trace_tick_handler

    global alice_handler_set
    if event == "call" and not alice_handler_set:
        alice_handler_set = True
        return trace_tick_handler_alice

    global bob_handler_set
    if event == "call" and not bob_handler_set:
        bob_handler_set = True
        return trace_tick_handler_bob

    return trace_tick_handler


def factorial(n):
    if n == 0:
        return 1
    else:
        return n * factorial(n - 1)


def do_tests():
    # These commands are here to demonstrate some execution being traced.
    print("Who loves the sun?")
    print("Not every-", factorial(3))

    from sys_settrace_subdir import trace_generic

    trace_generic.run_tests()
    return


sys.settrace(trace_tick_handler)
do_tests()
sys.settrace(None)

print("\n------------------ script exited ------------------")
print("Total traces executed: ", __prof__.trace_count)
