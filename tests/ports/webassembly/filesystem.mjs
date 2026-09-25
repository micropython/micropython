const mp = await (await import(process.argv[2])).loadMicroPython();

// Render a PyProxy identically across Node versions: newer Node's
// util.inspect annotates Proxy wrappers, so build the string from the
// stable _ref property instead of letting console.log render it.
const showPyProxy = (p) => `PyProxy { _ref: ${p._ref} }`;

mp.FS.mkdir("/lib/");
mp.FS.writeFile("/lib/testmod.py", "x = 1; print(__name__, x)");
mp.runPython("import testmod");

mp.runPython("import sys; sys.modules.clear()");
const testmod = mp.pyimport("testmod");
console.log("testmod:", showPyProxy(testmod), testmod.x);
