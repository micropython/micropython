# Test asyncio.gather() when a task is already finished before the gather starts.

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


# CPython and MicroPython differ in when they signal (and print) that a task raised an
# uncaught exception.  So define an empty custom_handler() to suppress this output.
def custom_handler(loop, context):
    pass


async def task_that_finishes_early(id, event, fail):
    print("task_that_finishes_early", id)
    event.set()
    if fail:
        raise ValueError("intentional exception", id)


async def task_that_runs():
    for i in range(5):
        print("task_that_runs", i)
        await asyncio.sleep(0)


async def main(start_task_that_runs, task_fail, return_exceptions):
    print("== start", start_task_that_runs, task_fail, return_exceptions)

    # Set exception handler to suppress exception output.
    loop = asyncio.get_event_loop()
    loop.set_exception_handler(custom_handler)

    # Create tasks.
    event_a = asyncio.Event()
    event_b = asyncio.Event()
    tasks = []
    if start_task_that_runs:
        tasks.append(asyncio.create_task(task_that_runs()))
    tasks.append(asyncio.create_task(task_that_finishes_early("a", event_a, task_fail)))
    tasks.append(asyncio.create_task(task_that_finishes_early("b", event_b, task_fail)))

    # Make sure task_that_finishes_early() are both done, before calling gather().
    await event_a.wait()
    await event_b.wait()

    # Gather the tasks.
    try:
        result = "complete", await asyncio.gather(*tasks, return_exceptions=return_exceptions)
    except Exception as er:
        result = "exception", er, start_task_that_runs and tasks[0].done()

    # Wait for the final task to finish (if it was started).
    if start_task_that_runs:
        await tasks[0]

    # Print results.
    print(result)


# Run the test in the 8 different combinations of its arguments.
for i in range(8):
    asyncio.run(main(bool(i & 4), bool(i & 2), bool(i & 1)))
