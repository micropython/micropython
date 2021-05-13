# test sys.settrace with while and for loops

import sys

try:
    sys.settrace
except AttributeError:
    print("SKIP")
    raise SystemExit


def print_stacktrace(frame, level=0):
    print(
        "%2d: %s@%s:%s => %s:%d"
        % (
            level,
            "  ",
            frame.f_globals["__name__"],
            frame.f_code.co_name,
            # Keep just the filename.
            "sys_settrace_" + frame.f_code.co_filename.split("sys_settrace_")[-1],
            frame.f_lineno,
        )
    )

    if frame.f_back:
        print_stacktrace(frame.f_back, level + 1)


trace_count = 0


def trace_tick_handler(frame, event, arg):
    global trace_count
    print("### trace_handler::main event:", event)
    trace_count += 1
    print_stacktrace(frame)
    return trace_tick_handler


def test_loop():
    # for loop
    r = 0
    for i in range(3):
        r += i
    print("test_for_loop", r)

    # while loop
    r = 0
    i = 0
    while i < 3:
        r += i
        i += 1
    print("test_while_loop", i)


sys.settrace(trace_tick_handler)
test_loop()
sys.settrace(None)
print("Total traces executed: ", trace_count)
