// Test the two REPL entries: replProcessChar() stays synchronous and
// returns a number, while replProcessCharWithAsyncify() lets code typed
// at the REPL call jsffi.run_sync(), queues characters typed while a
// line is suspended, and lets Ctrl-C interrupt a suspended line.  Only
// the REPL's own output lines of interest are printed.  Requires a jspi
// build on a JSPI-capable engine; skips otherwise.

const lines = [];
const mp = await (await import(process.argv[2])).loadMicroPython({
    stdout: (line) => lines.push(line),
});

await mp.runPythonAsync(
    "import jsffi\nimport js\njs.jspiProbe = jsffi.can_run_sync()",
);
if (!globalThis.jspiProbe) {
    console.log("SKIP");
    process.exit(0);
}

mp.replInit();

const typeSync = (text) => {
    let ret;
    for (const ch of text) {
        ret = mp.replProcessChar(ch.charCodeAt(0));
    }
    return ret;
};
const typeAsync = async (text) => {
    for (const ch of text) {
        await mp.replProcessCharWithAsyncify(ch.charCodeAt(0));
    }
};

console.log(typeof typeSync("import jsffi, js\r"));
typeSync("print('sync:', jsffi.can_run_sync())\r");
await typeAsync("print('async:', jsffi.run_sync(js.Promise.resolve(42)))\r");

// Characters typed while a line is suspended are queued: fire them
// without awaiting, as the CLI does, and both lines still complete in
// order.
globalThis.slow = () =>
    new Promise((resolve) => {
        setTimeout(() => resolve(7), 50);
    });
await typeAsync("print('slow:', jsffi.run_sync(js.slow()))");
const slowLine = mp.replProcessCharWithAsyncify("\r".charCodeAt(0));
let lastQueued;
for (const ch of "print('after:', 1)\r") {
    lastQueued = mp.replProcessCharWithAsyncify(ch.charCodeAt(0));
}
await slowLine;
await lastQueued;

// Ctrl-C typed while a line is suspended interrupts it at once, and the
// REPL carries on.
globalThis.never = new Promise(() => {});
await typeAsync("jsffi.run_sync(js.never)");
const neverLine = mp.replProcessCharWithAsyncify("\r".charCodeAt(0));
await new Promise((resolve) => {
    setTimeout(resolve, 20);
});
await mp.replProcessCharWithAsyncify(3);
await neverLine;
await typeAsync("print('alive:', 1)\r");

for (const line of lines) {
    const text = line.trimEnd();
    if (/^(sync|async|slow|after|alive):/.test(text)) {
        console.log(text);
    } else if (text.startsWith("KeyboardInterrupt")) {
        console.log("KeyboardInterrupt");
    }
}
