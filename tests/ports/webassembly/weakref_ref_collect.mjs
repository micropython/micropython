// Test weakref.ref() functionality requiring gc.collect().
// Should be kept in sync with tests/basics/weakref_ref_collect.py.
//
// This needs custom testing on the webassembly port since the GC can only
// run when Python code returns to JavaScript.

const mp = await (await import(process.argv[2])).loadMicroPython();

// Set up.
mp.runPython(`
import gc, weakref

class A:
    def __str__(self):
        return "<A object>"

def callback(r):
    print("callback", r())
`);

console.log("test basic use of ref() with only one argument");
mp.runPython(`
    a = A()
    r = weakref.ref(a)
    print(r())
    gc.collect()
`);
console.log("(outside Python)");
mp.runPython(`
    print(r())
    a = None
    gc.collect()
`);
console.log("(outside Python)");
mp.runPython(`
    print(r())
`);

console.log("test use of ref() with a callback");
mp.runPython(`
    a = A()
    r = weakref.ref(a, callback)
    print(r())
    gc.collect()
`);
console.log("(outside Python)");
mp.runPython(`
    print(r())
    a = None
    gc.collect()
`);
console.log("(outside Python)");
mp.runPython(`
    print(r())
`);

console.log("test when weakref gets collected before the object it refs");
mp.runPython(`
    a = A()
    r = weakref.ref(a, callback)
    print(r())
    r = None
    gc.collect()
`);
console.log("(outside Python)");
mp.runPython(`
    a = None
    gc.collect()
`);
