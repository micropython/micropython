// Test jsffi.create_proxy().

const mp = await (await import(process.argv[2])).loadMicroPython();

// Render a PyProxy identically across Node versions: newer Node's
// util.inspect annotates Proxy wrappers, so build the string from the
// stable _ref property instead of letting console.log render it.
const showPyProxy = (p) => `PyProxy { _ref: ${p._ref} }`;

mp.runPython(`
import jsffi
x = jsffi.create_proxy(1)
print(x)
y = jsffi.create_proxy([2])
print(y)
`);
console.log(mp.globals.get("x"));
console.log(mp.PyProxy.toJs(mp.globals.get("x")));
console.log(showPyProxy(mp.globals.get("y")));
console.log(mp.PyProxy.toJs(mp.globals.get("y")));
