// Test asyncio.create_task(), and tasks waiting on a Promise.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.p0 = new Promise((resolve, reject) => {
    resolve(123);
});

globalThis.p1 = new Promise((resolve, reject) => {
    setTimeout(() => {
        console.log("setTimeout resolved");
        resolve(456);
    }, 200);
});

mp.runPython(`
import js
import asyncio

async def task(id, promise):
    print("task start", id)
    print("task await", id, await promise)
    print("task await", id, await promise)
    print("task end", id)

print("start")
t1 = asyncio.create_task(task(1, js.p0))
t2 = asyncio.create_task(task(2, js.p1))
print("t1", t1.done(), t2.done())
print("end")
`);

// Wait for p1 to fulfill so t2 can continue.
await globalThis.p1;

// Wait a little longer so t2 can complete.
await new Promise((resolve, reject) => {
    setTimeout(resolve, 10);
});

mp.runPython(`
print("restart")
print("t1", t1.done(), t2.done())
`);
