// Test passing floats between JavaScript and Python.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.a = 1 / 2;
globalThis.b = Infinity;
globalThis.c = NaN;

mp.runPython(`
import js

# Test retrieving floats from JS.
print(js.a)
print(js.b)
print(js.c)

# Test calling JS which returns a float.
r = js.Math.random()
print(type(r), 0 < r < 1)

x = 1 / 2
y = float("inf")
z = float("nan")

# Test passing floats to a JS function.
js.console.log(x)
js.console.log(x, y)
js.console.log(x, y, z)
`);

// Test retrieving floats from Python.
console.log(mp.globals.get("x"));
console.log(mp.globals.get("y"));
console.log(mp.globals.get("z"));

// Test passing floats to a Python function.
const mp_print = mp.pyimport("builtins").print;
mp_print(globalThis.a);
mp_print(globalThis.a, globalThis.b);
mp_print(globalThis.a, globalThis.b, globalThis.c);

// Test calling Python which returns a float.
console.log(mp.pyimport("math").sqrt(0.16));
