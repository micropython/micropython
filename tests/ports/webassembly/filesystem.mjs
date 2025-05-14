const mp = await (await import(process.argv[2])).loadMicroPython();

mp.FS.mkdir("/lib/");
mp.FS.writeFile("/lib/testmod.py", "x = 1; print(__name__, x)");
mp.runPython("import testmod");

mp.runPython("import sys; sys.modules.clear()");
const testmod = mp.pyimport("testmod");
console.log("testmod:", testmod, testmod.x);
