// Smoke test for the cooperative VM yield (MICROPY_ENABLE_VM_YIELD): a
// compute-bound Python loop with no I/O must still hand the JS event loop
// turns - verified by a repeating timer that ticks *during* the loop, not only
// after it - and time.sleep() must yield rather than busy-spin. Needs a
// yield-capable (asyncify/jspi) build, so it skips on the plain synchronous one.

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
const assert = (cond, msg) => {
    if (!cond) throw new Error(msg);
};

// runPythonAsync runs to completion (a top-level expression returns None under
// FILE_INPUT, so assert inside Python rather than on the return value).
await mp.runPythonAsync("assert 21 * 2 == 42");

// A compute-bound loop with no I/O must still hand the event loop turns: a
// 20ms interval should tick several times *during* the ~600ms loop. A build
// that never yields could not run the interval until the loop returned, by
// which point it is cleared - leaving the count at zero. The bar is a handful
// of ticks, not a latency, so a loaded CI runner can't make it flaky.
let ticks = 0;
let iv = setInterval(() => {
    ticks++;
}, 20);
await mp.runPythonAsync(
    "import time\ns = time.ticks_ms()\nwhile time.ticks_ms() - s < 600:\n    pass\n",
);
clearInterval(iv);
assert(ticks >= 3, `tight loop did not yield (${ticks} event-loop ticks)`);

// time.sleep() should also yield rather than busy-spin.
ticks = 0;
iv = setInterval(() => {
    ticks++;
}, 20);
await mp.runPythonAsync("import time\ntime.sleep(0.5)");
clearInterval(iv);
assert(ticks >= 3, `time.sleep did not yield (${ticks} event-loop ticks)`);

console.log("yield ok");
