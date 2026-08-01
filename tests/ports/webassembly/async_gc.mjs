// Guards against unbounded heap growth in long-running loops under
// MICROPY_GC_SPLIT_HEAP_AUTO: gc_collect() defers collection to the top level,
// which a `while True:` program never reaches, so the heap grows without bound.
// A build with the cooperative VM yield collects mid-loop instead (see main.c
// gc_collect_if_pending()); this checks the heap stays bounded and that the
// collection is correct (long-lived data survives). Only meaningful on a
// yield-capable (asyncify/jspi) build, so it skips on the plain synchronous one.

import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

const module = await import(pathToFileURL(resolve(process.argv[2])));
if (!globalThis.__MICROPYTHON_ASYNC__) {
    console.log("SKIP");
    process.exit(0);
}

const mp = await module.loadMicroPython({
    heapsize: 7 * 1024 * 1024,
    stdout: () => {},
});
const mb = () => mp._module.HEAPU8.length / 1048576;
const start = mb();

// A never-returns-to-top-level loop that allocates garbage every iteration,
// keeps a running total, and stores long-lived data that must survive the
// mid-loop collection (a premature/unsafe collect would corrupt state).
await mp.runPythonAsync(`
keep = []
total = 0
i = 0
while True:
    r = (0, 0, 160, 120)              # tuple garbage
    s = "{:.1f}".format(i / 3)        # string garbage
    t = [i] * 40                      # list garbage
    total = (total + t[0] + len(s)) & 0xffffff
    if i % 25000 == 0:
        keep.append(i)                # long-lived; must NOT be collected
    i += 1
    if i >= 400000:
        break
assert keep == list(range(0, 400000, 25000)), "long-lived data corrupted"
`);

const grew = mb() - start;
if (grew >= 64) {
    throw new Error(`heap not bounded: grew ${grew.toFixed(0)} MB`);
}
console.log("bounded");
