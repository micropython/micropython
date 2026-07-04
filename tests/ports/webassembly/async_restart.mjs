// Stress the async entry points against rapid overlapping run/stop requests: an
// embedding that lets the user restart a script must never start a second VM op
// while one is in flight (Asyncify aborts with "cannot start an async operation
// when one is already in flight"; JSPI throws an unhandled WebAssembly.Exception).
// A minimal single-serialiser drives a tight interruptible loop and hammers the
// interleaving that breaks. Needs a yield-capable (asyncify/jspi) build.

import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

const module = await import(pathToFileURL(resolve(process.argv[2])));
if (!globalThis.__MICROPYTHON_ASYNC__) {
    console.log("SKIP");
    process.exit(0);
}

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
let aborted = null;
process.on("uncaughtException", (e) => {
    aborted = e;
});
process.on("unhandledRejection", (e) => {
    aborted = e;
});

const mp = await module.loadMicroPython({
    heapsize: 8 * 1024 * 1024,
    stdout: () => {},
});

// ---- a minimal single-serialiser around the port -------------------------
const STOP_TIMEOUT_MS = 1500;
let main = null;
let stopping = false;
let servicing = false;
let pending = null;

const runProgram = async (program) => {
    stopping = false;
    try {
        await mp.runPythonAsync(program);
    } catch (e) {
        if (!stopping) throw e; // a real error would surface; a stop is swallowed
    }
};

const stopCurrent = async () => {
    if (!main) return true;
    stopping = true;
    try {
        mp.interrupt();
    } catch (_) {}
    const stopped = await Promise.race([
        main.then(
            () => true,
            () => true,
        ),
        new Promise((r) => setTimeout(() => r(false), STOP_TIMEOUT_MS)),
    ]);
    if (stopped) {
        main = null;
        stopping = false;
    }
    return stopped;
};

const service = async () => {
    if (servicing) return;
    servicing = true;
    try {
        while (pending) {
            const req = pending;
            pending = null;
            if (!(await stopCurrent())) {
                pending = null;
                break;
            }
            if (req.kind === "run") {
                main = runProgram(req.program);
                main.catch(() => {});
            }
        }
    } finally {
        servicing = false;
    }
};

const LOOP = "n = 0\nwhile True:\n    n += 1\n";
const post = (req) => {
    pending = req;
    service().catch(() => {});
};

// ---- hammer the race ------------------------------------------------------
post({ kind: "run", program: LOOP });
await sleep(60);
for (let i = 0; i < 25 && !aborted; i++) {
    // Fire overlapping requests without awaiting, as rapid Run/Stop clicks do.
    post({ kind: "run", program: LOOP });
    post({ kind: "stop" });
    post({ kind: "run", program: LOOP });
    await sleep(30);
}
if (aborted) {
    throw new Error(String(aborted.message || aborted).split("\n")[0]);
}

// Settle, stop, and confirm the instance is still usable.
post({ kind: "stop" });
await sleep(200);
await mp.runPythonAsync("assert 1 + 1 == 2");

console.log("restart ok");
