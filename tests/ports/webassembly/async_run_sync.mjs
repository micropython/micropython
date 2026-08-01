// Smoke test for jsffi.run_sync(): synchronously block on a JS awaitable by
// suspending the stack, then return its resolved value (or raise its rejection).
// Needs a yield-capable (asyncify/jspi) build, so it skips on the plain one.

import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

const module = await import(pathToFileURL(resolve(process.argv[2])));
if (!globalThis.__MICROPYTHON_ASYNC__) {
    console.log("SKIP");
    process.exit(0);
}

const mp = await module.loadMicroPython({
    heapsize: 4 * 1024 * 1024,
    stdout: () => {},
});

await mp.runPythonAsync(`
import js
from jsffi import run_sync, can_run_sync

# run_sync is available inside a suspend-capable entry point.
assert can_run_sync(), "expected can_run_sync() to be True here"

# A resolved promise hands its value straight back, converted to a Python object.
assert run_sync(js.Promise.resolve(42)) == 42
assert run_sync(js.Promise.resolve("ok")) == "ok"

# A rejection surfaces as a raised exception carrying the JS error message.
try:
    run_sync(js.Promise.reject(js.Error("boom")))
    raise AssertionError("expected the rejection to raise")
except Exception as e:
    assert "boom" in str(e), repr(e)

# A non-awaitable argument is a TypeError, not a hang.
try:
    run_sync(123)
    raise AssertionError("expected TypeError for a non-awaitable")
except TypeError:
    pass
`);

console.log("run_sync ok");
