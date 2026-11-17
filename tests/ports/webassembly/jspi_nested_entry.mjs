// Test that jsffi.run_sync() refuses to suspend when its JSPI entry was
// started from inside another call into C.  Here a Python callback
// (a synchronous entry from the timer) calls runPythonAsync(), so that
// promising call is nested; suspending there would corrupt the outer
// call's state when it returns.  Requires a jspi build on a
// JSPI-capable engine; skips otherwise.

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(
    "import jsffi\nimport js\njs.jspiProbe = jsffi.can_run_sync()",
);
if (!globalThis.jspiProbe) {
    console.log("SKIP");
    process.exit(0);
}

globalThis.runNested = () =>
    mp.runPythonAsync(`
import jsffi
import js
print("inner: can_run_sync", jsffi.can_run_sync())
try:
    jsffi.run_sync(js.Promise.resolve(1))
except RuntimeError as exc:
    print("inner: RuntimeError", "no suspender" in str(exc))
`);

mp.runPython(`
import js

def outer():
    # Python calling back into runPythonAsync(): a nested entry.
    js.runNested()
    print("outer done")

js.setTimeout(outer, 0)
`);
await new Promise((resolve) => {
    setTimeout(resolve, 50);
});
