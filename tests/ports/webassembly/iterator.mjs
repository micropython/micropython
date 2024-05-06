// Test accessing Python iterables from JavaScript via the JavaScript iterator protocol.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
    s = "abc"
    l = [1, 2, 3]
`);

// Iterate a Python string.
for (const value of mp.globals.get("s")) {
    console.log(value);
}

// Iterate a Python list.
for (const value of mp.globals.get("l")) {
    console.log(value);
}

// Iterate a Python list from a built-in JavaScript constructor.
mp.runPython("import js; print(js.Set.new([1, 2, 3]).has(3))");
