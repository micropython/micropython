// Test graceful degradation on builds without JSPI: the functions
// still exist, can_run_sync() is False and run_sync() raises
// RuntimeError, so Pyodide-style feature detection is portable.
// Skips on jspi builds (where run_sync works).

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(
    "import jsffi\nimport js\njs.jspiProbe = jsffi.can_run_sync()",
);
if (globalThis.jspiProbe) {
    console.log("SKIP");
    process.exit(0);
}
delete globalThis.jspiProbe;

mp.runPython(`
import jsffi
print("can_run_sync:", jsffi.can_run_sync())
try:
    jsffi.run_sync(None)
except RuntimeError as exc:
    print("RuntimeError:", "not supported" in str(exc))
`);
