import sys

try:
    sys.settrace
except AttributeError:
    print("SKIP")
    raise SystemExit

if sys.version.startswith("3.12"):
    # There is a CPython change in settrace that is reverted in 3.13!
    print("WARNING: this test will fail when compared to CPython 3.12.x behaviour")


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
            # Keep just the filename.
            "sys_settrace_" + frame.f_code.co_filename.split("sys_settrace_")[-1],
            max(
                1, frame.f_lineno
            ),  # CPython 3.11 emits `0` where CPython 3.6 emits `1` for the "call" event corresponding to import.
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
    to_ignore = ["importlib", "zipimport", "encodings"]
    frame_name = frame.f_globals["__name__"]
    if any(name in frame_name for name in to_ignore):
        return

    # Lines 4,5,7 create the `const` lambda, and line `15` is a `_X = const()` which
    # MicroPython will not see as it's optimised out.
    if "sys_settrace_importme" in frame.f_code.co_filename and frame.f_lineno in (4, 5, 7, 15):
        return trace_tick_handler

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

    from sys_settrace_subdir import sys_settrace_generic

    sys_settrace_generic.run_tests()
    return


sys.settrace(trace_tick_handler)
do_tests()
sys.settrace(None)

print("\n------------------ script exited ------------------")
print("Total traces executed: ", __prof__.trace_count)
