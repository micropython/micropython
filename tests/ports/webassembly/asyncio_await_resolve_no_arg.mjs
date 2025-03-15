// Test an asyncio task await'ing on a Promise that's resolved without an argument.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.foo = new Promise((resolve) => {
    console.log(1);
    resolve(); // resolve without an argument
    console.log(2);
});

mp.runPython(`
import asyncio
import js

async def task():
    print(3)
    print(await js.foo)
    print(4)

asyncio.create_task(task())
`);
