// Test using Python functions as JS callbacks.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.asyncTimeout = (ms) =>
    new Promise((resolve) => setTimeout(resolve, ms));

mp.runPython(`
import js

def f0():
    print("f0 run")

async def f1():
    print("f1 run")

async def f2():
    print("f2 start")
    await js.asyncTimeout(0)
    print("f2 end")

async def f3():
    print("f3 start")
    await f2()
    print("f3 end")

js.setTimeout(f0, 0)
js.setTimeout(f1, 50)
js.setTimeout(f2, 100)
js.setTimeout(f3, 150)
`);
