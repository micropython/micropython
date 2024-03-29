// Test proxying of functions between Python and JavaScript.

const mp = await (await import(process.argv[2])).loadMicroPython();

// Create JS functions living on the JS and Py sides.
globalThis.jsFunAdd = (x, y) => x + y;
mp.globals.set("js_fun_sub", (x, y) => x - y);

console.log("== JavaScript side ==");

// JS function living on the JS side, should be a function.
console.log(globalThis.jsFunAdd);
console.log(globalThis.jsFunAdd(1, 2));

// JS function living on the Py side, should be a function.
console.log(mp.globals.get("js_fun_sub"));
console.log(mp.globals.get("js_fun_sub")(1, 2));

mp.runPython(`
import js

print("== Python side ==")

py_fun_mul = lambda x, y: x * y
js.pyFunDiv = lambda x, y: x / y

# JS function living on the JS side, should be a JsProxy.
print(type(js.jsFunAdd))
print(js.jsFunAdd(1, 2))

# JS function living on the Py side, should be a JsProxy.
print(type(js_fun_sub))
print(js_fun_sub(1, 2))

# Py function living on the Py side, should be a function.
print(type(py_fun_mul))
print(py_fun_mul(2, 3))

# Py function living on the JS side, should be a function.
print(type(js.pyFunDiv))
print(js.pyFunDiv(6, 2))
`);

console.log("== JavaScript side ==");

// Py function living on the Py side, should be a proxy function.
console.log(mp.globals.get("py_fun_mul"));
console.log(mp.globals.get("py_fun_mul")(2, 3));

// Py function living on the JS side, should be a proxy function.
console.log(globalThis.pyFunDiv);
console.log(globalThis.pyFunDiv(6, 2));
