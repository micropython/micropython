// Test JavaScript await'ing on Python async functions.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.asyncTimeout = (ms) =>
    new Promise((resolve) => setTimeout(resolve, ms));

mp.runPython(`
import js

def f0():
    print("f0 run")
    return 1

async def f1():
    print("f1 run")
    return 2

async def f2():
    print("f2 start")
    await js.asyncTimeout(0)
    print("f2 end")
    return 3

async def f3():
    print("f3 start")
    ret = await f2()
    print("f3 end")
    return ret + 1
`);

console.log("f0 return:", await mp.globals.get("f0")());
console.log("f1 return:", await mp.globals.get("f1")());
console.log("f2 return:", await mp.globals.get("f2")());
console.log("f3 return:", await mp.globals.get("f3")());
