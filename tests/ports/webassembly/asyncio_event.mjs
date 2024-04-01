// Test asyncio.Event.
//
// This test is taken from tests/extmod/asyncio_event.py and adapted:
// - changed asyncio.run(main()) to await main()
// - removed cancel and wait_for parts of the test

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(`
import asyncio

async def task(id, ev):
    print("start", id)
    print(await ev.wait())
    print("end", id)


async def main():
    ev = asyncio.Event()

    # Set and clear without anything waiting, and test is_set()
    print(ev.is_set())
    ev.set()
    print(ev.is_set())
    ev.clear()
    print(ev.is_set())

    # Create 2 tasks waiting on the event
    print("----")
    asyncio.create_task(task(1, ev))
    asyncio.create_task(task(2, ev))
    print("yield")
    await asyncio.sleep(0)
    print("set event")
    ev.set()
    print("yield")
    await asyncio.sleep(0)

    # Create a task waiting on the already-set event
    print("----")
    asyncio.create_task(task(3, ev))
    print("yield")
    await asyncio.sleep(0)

    # Clear event, start a task, then set event again
    print("----")
    print("clear event")
    ev.clear()
    asyncio.create_task(task(4, ev))
    await asyncio.sleep(0)
    print("set event")
    ev.set()
    await asyncio.sleep(0)


await main()
`);
