// Test identity of JsProxy when they are the same JavaScript object.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
import js

print(js.Object)

print("Object equality")
print(js.Object == js.Object)
print(js.Object.assign == js.Object.assign)

print("Array equality")
print(js.Array == js.Array)
print(js.Array.prototype == js.Array.prototype)
print(js.Array.prototype.push == js.Array.prototype.push)
`);
