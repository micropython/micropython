// Test jsffi.run_sync(): resolution, Python-awaitable arguments,
// rejection, and non-awaitable rejection.  Requires a jspi build on a
// JSPI-capable engine; skips otherwise.

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(
    "import jsffi\nimport js\njs.jspiProbe = jsffi.can_run_sync()",
);
if (!globalThis.jspiProbe) {
    console.log("SKIP");
    process.exit(0);
}
delete globalThis.jspiProbe;

globalThis.slowValue = new Promise((resolve) => {
    setTimeout(() => resolve(42), 10);
});

await mp.runPythonAsync(`
import jsffi
import js

# A JS promise resolves to its value.
print(jsffi.run_sync(js.slowValue))

# A Python awaitable (coroutine) is accepted, as in Pyodide.
async def answer():
    return 7

print(jsffi.run_sync(answer()))

# A non-awaitable is a TypeError.
try:
    jsffi.run_sync(123)
except TypeError:
    print("TypeError")

# A rejected promise raises JsException carrying the JS error.
try:
    jsffi.run_sync(js.Promise.reject(js.Error("boom")))
except jsffi.JsException as exc:
    print("JsException", "boom" in str(exc))
`);
