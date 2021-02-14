# Test current_task() function

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def task(result):
    result[0] = asyncio.current_task()


async def main():
    result = [None]
    t = asyncio.create_task(task(result))
    await asyncio.sleep(0)
    await asyncio.sleep(0)
    print(t is result[0])


asyncio.run(main())
