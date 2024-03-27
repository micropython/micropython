// Test asyncio.create_task().

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.p0 = new Promise((resolve, reject) => {
    resolve(123);
});

globalThis.p1 = new Promise((resolve, reject) => {
    setTimeout(() => {
        console.log("setTimeout resolved");
        resolve(456);
    }, 500);
});

mp.runPython(`
import js
import asyncio

async def task(id, pp):
    print("task start", id)
    print("task await", id, await pp)
    print("task await", id, await pp)
    print("task end", id)

print("start")
t1 = asyncio.create_task(task(1, js.p0))
t2 = asyncio.create_task(task(2, js.p1))
print("t1", t1.done(), t2.done())
print("end")
`);

await globalThis.p1;
await globalThis.p1;
await globalThis.p1;
await globalThis.p1;

mp.runPython(`
print("restart")
print("t1", t1.done(), t2.done())
`);
