// Test PyProxy.toJs().

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
a = 1
b = (1, 2, 3)
c = [None, True, 1.2]
d = {"one": 1, "tuple": b, "list": c}
`);

const py_a = mp.globals.get("a");
const py_b = mp.globals.get("b");
const py_c = mp.globals.get("c");
const py_d = mp.globals.get("d");

console.log(py_a instanceof mp.PyProxy, mp.PyProxy.toJs(py_a));
console.log(py_b instanceof mp.PyProxy, mp.PyProxy.toJs(py_b));
console.log(py_c instanceof mp.PyProxy, mp.PyProxy.toJs(py_c));
console.log(py_d instanceof mp.PyProxy, mp.PyProxy.toJs(py_d));
