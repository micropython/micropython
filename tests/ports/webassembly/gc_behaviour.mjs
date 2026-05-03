// Test GC behaviour, that objects are not prematurely collected.

const mp = await (await import(process.argv[2])).loadMicroPython();

// This test converts a Python list/dict to the corresponding JavaScript object and
// returns it to JavaScript.  Internally it will be wrapped in a `JsProxy`, and the
// call to `gc.collect()` will force a GC collection upon exit of the `make_xxx()`
// function call but before the `JsProxy` is passed to JavaScript.  The test ensures
// that the `JsProxy` doesn't get prematurely collected before JavaScript has a chance
// to receive the underlying JavaScript object.

mp.runPython(`
import gc, js, jsffi

def make_list():
    obj = jsffi.to_js([1, 2, 3])
    print(obj)
    js.console.log(obj)
    gc.collect()
    return obj

def make_dict():
    obj = jsffi.to_js({"key": "value"})
    print(obj)
    js.console.log(obj)
    gc.collect()
    return obj
`);

console.log(mp.globals.get("make_list")());
console.log(mp.globals.get("make_dict")());
