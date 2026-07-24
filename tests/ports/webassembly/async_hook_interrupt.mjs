// Guards the node-stdin Ctrl-C path. mp_js_hook (library.js) is polled from the
// cooperative VM yield (main.c mp_js_yield), so a Ctrl-C byte arriving on stdin
// during a running script must raise KeyboardInterrupt even though the script
// never awaits. This is a regression guard: the yield hook took over the VM hook
// from the plain JS_HOOK path, and it must keep polling mp_js_hook or interactive
// Ctrl-C silently stops working.
//
// A child process enables Ctrl-C (micropython.kbd_intr) and spins in Python; the
// parent injects the interrupt byte on the child's stdin. If the hook were not
// polled the byte would go unread and the loop would never stop, so the parent's
// watchdog would kill the child and the KeyboardInterrupt line would be absent.
//
// The stdin hook is compiled in by the asyncify variants (MICROPY_VARIANT_ENABLE_
// JS_HOOK); it skips on the plain synchronous build and on jspi (no JS_HOOK).

import { spawn } from "node:child_process";
import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

const mjs = resolve(process.argv[2]);

if (process.argv[3] === "--child") {
    const mod = await import(pathToFileURL(mjs));
    const mp = await mod.loadMicroPython({
        heapsize: 8 * 1024 * 1024,
        stdout: () => {},
    });
    // Enable Ctrl-C as the interrupt character (disabled by default outside the
    // REPL), then spin so the VM yield keeps polling stdin via mp_js_hook.
    mp.runPython("import micropython; micropython.kbd_intr(3)");
    try {
        await mp.runPythonAsync("while True:\n    pass\n");
        console.log("no-interrupt");
    } catch (e) {
        console.log(e.type === "KeyboardInterrupt" ? "kbd-interrupt" : "other");
    }
    process.exit(0);
}

// Only the asyncify variants compile the stdin hook; skip elsewhere.
await import(pathToFileURL(mjs));
if (globalThis.__MICROPYTHON_ASYNC__ !== "asyncify") {
    console.log("SKIP");
    process.exit(0);
}

const child = spawn(process.execPath, [process.argv[1], mjs, "--child"], {
    stdio: ["pipe", "pipe", "inherit"],
});
let out = "";
child.stdout.on("data", (d) => {
    out += d;
});
// Watchdog: if the interrupt never lands the child loops forever, so kill it and
// let the missing "kbd-interrupt" fail the test rather than hang the run.
const watchdog = setTimeout(() => child.kill("SIGKILL"), 10000);
// Give the child time to start the loop, then deliver Ctrl-C on its stdin.
setTimeout(() => child.stdin.write(Buffer.from([3])), 500);

const ok = await new Promise((res) => {
    child.on("exit", () => res(out.includes("kbd-interrupt")));
});
clearTimeout(watchdog);
console.log(ok ? "hook interrupt ok" : `hook interrupt FAILED: ${out.trim()}`);
