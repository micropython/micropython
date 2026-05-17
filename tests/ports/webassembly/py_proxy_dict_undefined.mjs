// Test passing a Python dict into JavaScript, how it behaves with undefined keys.
// If JavaScript accesses a key that does not exist, `undefined` should be returned.
// This is different to Python-side behaviour, where `KeyError` is raised.

const mp = await (await import(process.argv[2])).loadMicroPython();

// Create a JavaScript function with default arguments.
// When `value` is `undefined` it will receive its default.
function withDefault({ value = "OK" } = {}) {
    console.log(value);
}

globalThis.withDefault = withDefault;

// Call the function from JavaScript with various arguments.
withDefault();
withDefault({});
withDefault({ value: null });
withDefault({ value: undefined });
withDefault({ value: () => {} });

console.log("====");

// Call the function from Python with the same arguments as above.
// The results should be the same.
mp.runPython(`
import js

js.withDefault()
js.withDefault({})
js.withDefault({"value": None})
js.withDefault({"value": js.undefined})
js.withDefault({"value": (lambda: {})})
`);
