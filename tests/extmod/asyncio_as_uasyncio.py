try:
    import uasyncio
except ImportError:
    print("SKIP")
    raise SystemExit

x = set(dir(uasyncio))
y = set(dir(uasyncio)) - set(["event", "lock", "stream", "funcs", "taskgroup"])

# Sample of public symbols we expect to see from `asyncio`. Verify they're all
# available on `uasyncio`.
expected = [
    "CancelledError",
    "create_task",
    "current_task",
    "Event",
    "gather",
    "get_event_loop",
    "Lock",
    "Loop",
    "open_connection",
    "run",
    "run_until_complete",
    "sleep",
    "sleep_ms",
    "start_server",
    "StreamReader",
    "StreamWriter",
    "Task",
    "ThreadSafeFlag",
    "wait_for",
]

for e in expected:
    getattr(uasyncio, e)
