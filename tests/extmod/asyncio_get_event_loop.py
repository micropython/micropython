# Test get_event_loop()

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

# CPython 3.12 deprecated calling get_event_loop() when there is no current event
# loop, so to make this test run on CPython requires setting the event loop.
if hasattr(asyncio, "set_event_loop"):
    asyncio.set_event_loop(asyncio.new_event_loop())


async def main():
    print("start")
    await asyncio.sleep(0.01)
    print("end")


loop = asyncio.get_event_loop()
loop.run_until_complete(main())
