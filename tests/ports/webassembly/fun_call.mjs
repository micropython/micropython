// Test calling JavaScript functions from Python.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.f = (a, b, c, d, e) => {
    console.log(a, b, c, d, e);
};
mp.runPython(`
import js
js.f()
js.f(1)
js.f(1, 2)
js.f(1, 2, 3)
js.f(1, 2, 3, 4)
js.f(1, 2, 3, 4, 5)
js.f(1, 2, 3, 4, 5, 6)
`);
