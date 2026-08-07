// Test that KeyboardInterrupt breaks a pending run_sync(): pending
// events are serviced between suspension polls.

const mp = await (await import(process.argv[2])).loadMicroPython();

await mp.runPythonAsync(
    "import jsffi\nimport js\njs.jspiProbe = jsffi.can_run_sync()",
);
if (!globalThis.jspiProbe) {
    console.log("SKIP");
    process.exit(0);
}
delete globalThis.jspiProbe;

globalThis.never = new Promise(() => {});

setTimeout(() => {
    mp._module.ccall("mp_sched_keyboard_interrupt", "null", [], []);
}, 50);

await mp.runPythonAsync(`
import jsffi
import js
try:
    jsffi.run_sync(js.never)
    print("not reached")
except KeyboardInterrupt:
    print("KeyboardInterrupt")
`);
