// Test jsffi.create_proxy().

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
import jsffi
x = jsffi.create_proxy(1)
print(x)
y = jsffi.create_proxy([2])
print(y)
`);
console.log(mp.globals.get("x"));
console.log(mp.PyProxy.toJs(mp.globals.get("x")));
console.log(mp.globals.get("y"));
console.log(mp.PyProxy.toJs(mp.globals.get("y")));
