# Test the Task.set_result() method

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task():
    pass


async def main():
    # Task.set_exception must raise an error
    print("=" * 10)
    t = asyncio.create_task(task())
    try:
        t.set_result(None)
    except Exception as e:
        print("Task.set_result is not callable:", repr(e))


asyncio.run(main())
