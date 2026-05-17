// Test `Reflect.ownKeys(<py-obj>)` on the JavaScript side, which tests PyProxy.ownKeys.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
x = []
y = {"a": 1}
`);

console.log(Reflect.ownKeys(mp.globals.get("x")));
console.log(Reflect.ownKeys(mp.globals.get("y")));
