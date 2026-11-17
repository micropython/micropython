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

globalThis.g = (...args) => {
    console.log(args);
};
mp.runPython(`
import js
js.g()
js.g(a=1)
js.g(a=1, b=2)
js.g(a=1, b=2, c=3)
js.g(a=1, b=2, c=3, d=4)
js.g(a=1, b=2, c=3, d=4, e=5)
js.g(1)
js.g(1, b=2)
js.g(1, b=2, c=3)
js.g(1, b=2, c=3, d=4)
js.g(1, b=2, c=3, d=4, e=5)
js.g(1, 2)
js.g(1, 2, c=3)
js.g(1, 2, c=3, d=4)
js.g(1, 2, c=3, d=4, e=5)
js.g(1, 2, 3)
js.g(1, 2, 3, d=4)
js.g(1, 2, 3, d=4, e=5)
js.g(1, 2, 3, 4)
js.g(1, 2, 3, 4, e=5)
`);
