// Test jsffi.run_sync() inside asyncio tasks.  The asyncio loop runs
// through callPromising(), so a task may suspend; while it is suspended
// the loop waits, and other tasks run once it resumes.  Requires a jspi
// build on a JSPI-capable engine; skips otherwise.

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(
    "import jsffi\nimport js\njs.jspiProbe = jsffi.can_run_sync()",
);
if (!globalThis.jspiProbe) {
    console.log("SKIP");
    process.exit(0);
}

globalThis.slowValue = new Promise((resolve) => {
    setTimeout(() => resolve(42), 10);
});
globalThis.never = new Promise(() => {});

// The first task suspends in run_sync(); the second runs after it.
mp.runPython(`
import asyncio
import js
import jsffi

async def first():
    print("first: can_run_sync", jsffi.can_run_sync())
    print("first:", jsffi.run_sync(js.slowValue))
    print("first done")

async def second():
    print("second done")

asyncio.create_task(first())
asyncio.create_task(second())
`);
await new Promise((resolve) => {
    setTimeout(resolve, 50);
});

// A KeyboardInterrupt while a task is suspended is raised in that task,
// and the loop carries on with the next task.
setTimeout(() => {
    mp._module.ccall("mp_sched_keyboard_interrupt", "null", [], []);
}, 20);
mp.runPython(`
async def interrupted():
    try:
        jsffi.run_sync(js.never)
    except KeyboardInterrupt:
        print("interrupted: KeyboardInterrupt")

async def after():
    print("after done")

asyncio.create_task(interrupted())
asyncio.create_task(after())
`);
await new Promise((resolve) => {
    setTimeout(resolve, 100);
});
