// Test the single-suspension rule: while one run_sync() is parked, a
// second overlapping promising entry may run but its run_sync() is
// refused with a distinct RuntimeError.

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(
    "import jsffi\nimport js\njs.jspiProbe = jsffi.can_run_sync()",
);
if (!globalThis.jspiProbe) {
    console.log("SKIP");
    process.exit(0);
}
delete globalThis.jspiProbe;

let release;
globalThis.gate = new Promise((resolve) => {
    release = resolve;
});

// First entry parks on the gate.
const first = mp.runPythonAsync(`
import jsffi
import js
print("first:", jsffi.run_sync(js.gate))
`);

// Let the first suspension settle in.
await new Promise((resolve) => setTimeout(resolve, 10));

// Second entry runs while the first is parked; its run_sync must be
// refused (and can_run_sync must say so beforehand).
await mp.runPythonAsync(`
import jsffi
import js
print("second can_run_sync:", jsffi.can_run_sync())
try:
    jsffi.run_sync(js.Promise.resolve(1))
except RuntimeError as exc:
    print("second RuntimeError:", "pending" in str(exc))
`);

release(99);
await first;
