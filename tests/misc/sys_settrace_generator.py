# test sys.settrace with generators

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


def test_generator():
    def make_gen():
        yield 1 << 0
        yield 1 << 1
        yield 1 << 2
        return 1 << 3

    gen = make_gen()
    r = 0
    try:

        r += gen.send(None)

        while True:

            r += gen.send(None)

    except StopIteration as e:
        print("test_generator", r, e)

    gen = make_gen()
    r = 0
    for i in gen:
        r += i
    print(r)


sys.settrace(trace_tick_handler)
test_generator()
sys.settrace(None)
print("Total traces executed: ", trace_count)
