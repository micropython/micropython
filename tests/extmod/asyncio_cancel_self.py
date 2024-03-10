# Test a task cancelling itself (currently unsupported)

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task():
    print("task start")
    global_task.cancel()


async def main():
    global global_task
    global_task = asyncio.create_task(task())
    try:
        await global_task
    except asyncio.CancelledError:
        print("main cancel")
    print("main done")


try:
    asyncio.run(main())
except RuntimeError as er:
    print(er)
