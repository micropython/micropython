// Exercises the in-place stop primitive: mp.interrupt() raises KeyboardInterrupt
// in a running script (a tight loop and a sliced time.sleep), and the instance
// stays usable afterwards with no teardown. Needs a yield-capable (asyncify/jspi)
// build, so it skips on the plain synchronous one.

import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

const module = await import(pathToFileURL(resolve(process.argv[2])));
if (!globalThis.__MICROPYTHON_ASYNC__) {
    console.log("SKIP");
    process.exit(0);
}

const mp = await module.loadMicroPython({
    heapsize: 8 * 1024 * 1024,
    stdout: () => {},
});

const assert = (cond, msg) => {
    if (!cond) throw new Error(msg);
};
// Run a snippet; report whether it raised and (if so) the exception type.
const run = async (src) => {
    try {
        await mp.runPythonAsync(src);
        return { ok: true };
    } catch (e) {
        return { ok: false, type: e.type };
    }
};

// Interrupt a tight Python loop.
{
    const p = run("x = 0\nwhile True:\n    x += 1\n");
    setTimeout(() => mp.interrupt(), 150);
    const r = await p;
    assert(
        !r.ok && r.type === "KeyboardInterrupt",
        `tight loop not interrupted: ${r.type || "no error"}`,
    );
    // Instance must still be alive and usable after the interrupt.
    assert(
        (await run("assert 1 + 1 == 2")).ok,
        "instance unusable after interrupt",
    );
    assert(
        (await run(`import json\nassert json.dumps({"a": 1}) == '{"a": 1}'`))
            .ok,
        "imports broken after interrupt",
    );
}

// Interrupt time.sleep (sliced, so it stops promptly rather than after 10s).
{
    const t0 = Date.now();
    const p = run("import time\ntime.sleep(10)");
    setTimeout(() => mp.interrupt(), 150);
    const r = await p;
    const dt = Date.now() - t0;
    assert(
        !r.ok && r.type === "KeyboardInterrupt",
        `time.sleep not interrupted: ${r.type || "no error"}`,
    );
    assert(dt < 1000, `time.sleep interrupt too slow: ${dt}ms`);
}

console.log("interrupt ok");
