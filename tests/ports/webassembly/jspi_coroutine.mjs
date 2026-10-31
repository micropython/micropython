// Test jsffi.run_sync() inside runPythonAsync() code that awaits.  Such
// code is a generator resumed from JS promise callbacks, so this checks
// that those resumes are promising entries.  Requires a jspi build on a
// JSPI-capable engine; skips otherwise.

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

// run_sync() works both before and after an await.
await mp.runPythonAsync(`
import jsffi
import js
print("can_run_sync before await:", jsffi.can_run_sync())
print("run_sync before await:", jsffi.run_sync(js.slowValue))
print("awaited:", await js.Promise.resolve(1))
print("can_run_sync after await:", jsffi.can_run_sync())
print("run_sync after await:", jsffi.run_sync(js.slowValue))
`);

// A KeyboardInterrupt while suspended after an await rejects the
// promise returned by runPythonAsync().
setTimeout(() => {
    mp._module.ccall("mp_sched_keyboard_interrupt", "null", [], []);
}, 50);
try {
    await mp.runPythonAsync(`
import jsffi
import js
await js.Promise.resolve(None)
jsffi.run_sync(js.never)
print("not reached")
`);
} catch (error) {
    console.log("rejected:", error.name, error.type);
}
