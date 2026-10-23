// Test the entry rule: suspension is legal only inside the promising
// entry (runPythonAsync); runPython is synchronous and run_sync there
// raises RuntimeError, matching Pyodide.

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(
    "import jsffi\nimport js\njs.jspiProbe = jsffi.can_run_sync()",
);
if (!globalThis.jspiProbe) {
    console.log("SKIP");
    process.exit(0);
}

await mp.runPythonAsync(`
import jsffi
print("async entry:", jsffi.can_run_sync())
`);

mp.runPython(`
import jsffi
print("sync entry:", jsffi.can_run_sync())
try:
    jsffi.run_sync(None)
except RuntimeError as exc:
    print("RuntimeError:", "no suspender" in str(exc))
`);
