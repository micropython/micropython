// Test top-level await on asyncio primitives: Task, Event.

const mp = await (await import(process.argv[2])).loadMicroPython();

/**********************************************************/
// Top-level await for an Event which is set by a JavaScript
// callback.

console.log("= TEST 1 ==========");

await mp.runPythonAsync(`
import asyncio
import js

event = asyncio.Event()

def callback():
    print("callback set event")
    event.set()

js.setTimeout(callback, 100)

print("top-level wait event")
await event.wait()
print("top-level end")
`);

console.log("finished");

/**********************************************************/
// Top-level await for a Task which is cancelled by a
// JavaScript callback.

console.log("= TEST 2 ==========");

await mp.runPythonAsync(`
import asyncio
import js
import time

async def task():
    print("task start")
    await asyncio.sleep(5)
    print("task end")

def callback():
    print("callback cancel task")
    t.cancel()

t  = asyncio.create_task(task())
js.setTimeout(callback, 100)

print("top-level wait task")
try:
    t0 = time.time()
    await t
except asyncio.CancelledError:
    dt = time.time() - t0
    print("top-level task CancelledError", dt < 1)
`);

console.log("finished");

/**********************************************************/
// Top-level await for an Event and a Task, with the task
// setting the event.

console.log("= TEST 3 ==========");

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
