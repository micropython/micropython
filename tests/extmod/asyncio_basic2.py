try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def forever():
    print("forever start")
    await asyncio.sleep(10)


async def main():
    print("main start")
    asyncio.create_task(forever())
    await asyncio.sleep(0.001)
    print("main done")
    return 42


print(asyncio.run(main()))
