# Test get_event_loop()

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def main():
    print("start")
    await asyncio.sleep(0.01)
    print("end")


loop = asyncio.get_event_loop()
loop.run_until_complete(main())
