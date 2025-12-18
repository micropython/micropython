// Test `<attr> in <py-obj>` on the JavaScript side, which tests PyProxy.has.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
x = []
`);

const x = mp.globals.get("x");
console.log("no_exist" in x);
console.log("sort" in x);
console.log(Symbol.toStringTag in x);
console.log(Symbol.iterator in x);
