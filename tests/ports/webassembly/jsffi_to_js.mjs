// Test jsffi.to_js().

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
import jsffi
x = jsffi.to_js(1)
print(x)
y = jsffi.to_js([2])
print(y)
z = jsffi.to_js({"three":3})
print(z)
`);

const x = mp.globals.get("x");
const y = mp.globals.get("y");
const z = mp.globals.get("z");

console.log(Array.isArray(x));
console.log(x);

console.log(Array.isArray(y));
console.log(y);
console.log(Reflect.ownKeys(y));

console.log(Array.isArray(z));
console.log(z);
console.log(Reflect.ownKeys(z));
