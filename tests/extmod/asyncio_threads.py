# Test asyncio.wait_for

try:
    import asyncio
    import _thread
except ImportError:
    print("SKIP")
    raise SystemExit

from time import sleep_ms


def print_abc():
    print("A")
    sleep_ms(50)
    print("B")
    sleep_ms(50)
    print("C")
    return 42


async def main():
    t = asyncio.create_task(asyncio.to_thread(print_abc))
    await asyncio.sleep_ms(25)
    print("X")
    await asyncio.sleep_ms(50)
    print("Z")

    print(await t)
    print("finish")


asyncio.run(main())
