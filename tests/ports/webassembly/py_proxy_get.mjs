// Test `<py-obj> get <attr>` on the JavaScript side, which tests PyProxy.get.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
x = {"a": 1}
`);

const x = mp.globals.get("x");
console.log(x.a === 1);
console.log(x.b === undefined);
console.log(typeof x[Symbol.iterator] === "function");
console.log(x[Symbol.toStringTag] === undefined);
console.log(x.then === undefined);
