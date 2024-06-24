// Test lookup of attributes on JsProxy objects.

const mp = await (await import(process.argv[2])).loadMicroPython();

// Simple attribute names and values.
globalThis.obj1 = { a: 1, b: 2 };

// Unconventional attribute names and values.
globalThis.obj2 = { undefined: "undefined", undef: undefined };

// Dynamically created attribute names and values.
globalThis.obj3 = new Proxy(new Map(), {
    get(map, name) {
        if (!map.has(name)) {
            console.log("creating attribute", name);
            map.set(name, name);
        }
        return map.get(name);
    },
});

mp.runPython(`
import js

print(js.obj1.a, js.obj1.b)
print(js.obj1["a"], js.obj1["b"])

print(js.obj2.undefined, js.obj2.undef)

print(js.obj3.c)
print(js.obj3["c"])
print(hasattr(js.obj3, "d"))
print(js.obj3.d)
`);
