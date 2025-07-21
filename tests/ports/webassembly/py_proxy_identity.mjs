// Test identity of PyProxy when they are the same Python object.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
l = []
`);

const l1 = mp.globals.get("l");
const l2 = mp.globals.get("l");
console.log(l1, l2);
console.log(l1 === l2);

globalThis.eventTarget = new EventTarget();
globalThis.event = new Event("event");

mp.runPython(`
import js

def callback(ev):
    print("callback", ev)
js.eventTarget.addEventListener("event", callback)
js.eventTarget.dispatchEvent(js.event)
js.eventTarget.removeEventListener("event", callback)
js.eventTarget.dispatchEvent(js.event)

print("Object equality")
print(js.Object == js.Object)
print(js.Object.assign == js.Object.assign)

print("Array equality")
print(js.Array == js.Array)
print(js.Array.prototype == js.Array.prototype)
print(js.Array.prototype.push == js.Array.prototype.push)
`);
