// Test passing a Python dict into JavaScript, it should act like a JS object.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
x = {"a": 1, "b": 2}
`);

const x = mp.globals.get("x");

// Test has, get, keys/iteration.
console.log("a" in x, "b" in x, "c" in x);
console.log(x.a, x.b);
for (const k in x) {
    console.log(k, x[k]);
}
console.log(Object.keys(x));
console.log(Reflect.ownKeys(x));

// Test set.
x.c = 3;
console.log(Object.keys(x));

// Test delete.
// biome-ignore lint/performance/noDelete: test delete statement
delete x.b;
console.log(Object.keys(x));

// Make sure changes on the JavaScript side are reflected in Python.
mp.runPython(`
print(x["a"])
print("b" in x)
print(x["c"])
`);
