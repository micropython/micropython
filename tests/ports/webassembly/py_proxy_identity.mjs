// Test identity of PyProxy when they are the same Python object.

const mp = await (await import(process.argv[2])).loadMicroPython();

// Render a PyProxy identically across Node versions: newer Node's
// util.inspect annotates Proxy wrappers, so build the string from the
// stable _ref property instead of letting console.log render it.
const showPyProxy = (p) => `PyProxy { _ref: ${p._ref} }`;

mp.runPython(`
l = []
`);

const l1 = mp.globals.get("l");
const l2 = mp.globals.get("l");
console.log(showPyProxy(l1), showPyProxy(l2));
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
`);
