// Test asyncio.sleep(), both at the top level and within a task.

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(`
import time
import asyncio

print("main start")
t0 = time.time()
await asyncio.sleep(0.25)
dt = time.time() - t0
print(0.2 <= dt <= 0.3)

async def task():
    print("task start")
    t0 = time.time()
    await asyncio.sleep(0.25)
    dt = time.time() - t0
    print(0.2 <= dt <= 0.3)
    print("task end")

asyncio.create_task(task())
print("main end")
`);
