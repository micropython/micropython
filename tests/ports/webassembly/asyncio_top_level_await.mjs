// Test top-level await on asyncio primitives: Task, Event.

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(`
import asyncio

async def task(event):
    print("task set event")
    event.set()
    print("task sleep")
    await asyncio.sleep(0.1)
    print("task end")

event = asyncio.Event()
t = asyncio.create_task(task(event))

print("top-level wait event")
await event.wait()
print("top-level wait task")
await t
print("top-level end")
`);

console.log("finished");
