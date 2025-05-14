// Test runPythonAsync() and top-level await in Python.

const mp = await (await import(process.argv[2])).loadMicroPython();

/**********************************************************/
// Using only promise objects, no await's.

console.log("= TEST 1 ==========");

globalThis.p = new Promise((resolve, reject) => {
    resolve(123);
});

console.log(1);

mp.runPython(`
import js
print(js.p)
print("py 1")
print(js.p.then(lambda x: print("resolved", x)))
print("py 2")
`);

console.log(2);

// Let the promise resolve.
await globalThis.p;

console.log(3);

/**********************************************************/
// Using setTimeout to resolve the promise.

console.log("= TEST 2 ==========");

globalThis.p = new Promise((resolve, reject) => {
    setTimeout(() => {
        resolve(123);
        console.log("setTimeout resolved");
    }, 100);
});

console.log(1);

mp.runPython(`
import js
print(js.p)
print("py 1")
print(js.p.then(lambda x: print("resolved", x)))
print("py 2")
`);

console.log(2);

// Let the promise resolve.
await globalThis.p;

console.log(3);

/**********************************************************/
// Using setTimeout and await within Python.

console.log("= TEST 3 ==========");

globalThis.p = new Promise((resolve, reject) => {
    setTimeout(() => {
        resolve(123);
        console.log("setTimeout resolved");
    }, 100);
});

console.log(1);

const ret3 = await mp.runPythonAsync(`
import js
print("py 1")
print("resolved value:", await js.p)
print("py 2")
`);

console.log(2, ret3);

/**********************************************************/
// Multiple setTimeout's and await's within Python.

console.log("= TEST 4 ==========");

globalThis.p1 = new Promise((resolve, reject) => {
    setTimeout(() => {
        resolve(123);
        console.log("setTimeout A resolved");
    }, 100);
});

globalThis.p2 = new Promise((resolve, reject) => {
    setTimeout(() => {
        resolve(456);
        console.log("setTimeout B resolved");
    }, 200);
});

console.log(1);

const ret4 = await mp.runPythonAsync(`
import js
print("py 1")
print("resolved value:", await js.p1)
print("py 2")
print("resolved value:", await js.p1)
print("py 3")
print("resolved value:", await js.p2)
print("py 4")
`);

console.log(2, ret4);
