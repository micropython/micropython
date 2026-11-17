// Test reuse of JsProxy references and freeing of JsProxy objects.
// This ensures that a Python-side JsProxy that refers to a JavaScript object retains
// the correct JavaScript object in the case that another JsProxy that refers to the
// same JavaScript object is freed.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.obj = [1, 2];
globalThis.obj2 = [3, 4];

console.log("JS obj:", globalThis.obj);

mp.runPython(`
import gc
import js

# Create 2 proxies of the same JS object.
# They should refer to the same underlying JS-side reference.
obj = js.obj
obj_copy = js.obj
print(obj, obj_copy, obj == obj_copy)

# Print out the object.
js.console.log("Py obj:", obj)

# Forget obj_copy and trigger a GC when the Python code finishes.
obj_copy = None
gc.collect()
`);

console.log("JS obj:", globalThis.obj);

mp.runPython(`
# Create a new proxy of a different object.
# It should not clobber the existing obj proxy reference.
obj2 = js.obj2

# Create a copy of the existing obj proxy.
obj_copy = js.obj

# Print the JS proxy, it should be the same reference as before.
print(obj, obj_copy, obj == obj_copy)

# Print out the object.
js.console.log("Py obj:", obj)
`);

console.log("JS obj:", globalThis.obj);
