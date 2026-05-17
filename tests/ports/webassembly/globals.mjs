// Test accessing Python globals dict via mp.globals.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython("x = 1");
console.log(mp.globals.get("x"));

mp.globals.set("y", 2);
mp.runPython("print(y)");

mp.runPython("print('y' in globals())");
mp.globals.delete("y");
mp.runPython("print('y' in globals())");
