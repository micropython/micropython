// Test runPythonAsync() without await'ing it.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.p = new Promise((resolve, reject) => {
    setTimeout(() => {
        resolve(123);
        console.log("setTimeout resolved");
    }, 100);
});

console.log(1);

const ret = mp.runPythonAsync(`
import js
print("py 1")
print("resolved value:", await js.p)
print("py 2")
`);

// `ret` should be a Promise.
console.log(2, ret);

// Here, the Python async code should continue to run until completed.
