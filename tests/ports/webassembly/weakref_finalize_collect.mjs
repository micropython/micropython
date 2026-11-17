// Test weakref.finalize() functionality requiring gc.collect().
// Should be kept in sync with tests/basics/weakref_finalize_collect.py.
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

def callback(*args, **kwargs):
    print("callback({}, {})".format(args, kwargs))
    return 42
`);

console.log("test basic use of finalize() with a simple callback");
mp.runPython(`
    a = A()
    f = weakref.finalize(a, callback)
    a = None
    gc.collect()
`);
console.log("(outside Python)");
mp.runPython(`
    print("alive", f.alive)
    print("peek", f.peek())
    print("detach", f.detach())
    print("call", f())
`);

console.log("test that a callback is passed the correct values");
mp.runPython(`
    a = A()
    f = weakref.finalize(a, callback, 1, 2, kwarg=3)
    a = None
    gc.collect()
`);
console.log("(outside Python)");
mp.runPython(`
    print("alive", f.alive)
    print("peek", f.peek())
    print("detach", f.detach())
    print("call", f())
`);

console.log("test that calling the finalizer cancels the finalizer");
mp.runPython(`
    a = A()
    f = weakref.finalize(a, callback)
    print(f())
    print(a)
    a = None
    gc.collect()
`);
console.log("(outside Python)");

console.log("test that calling detach cancels the finalizer");
mp.runPython(`
    a = A()
    f = weakref.finalize(a, callback)
    print(len(f.detach()))
    print(a)
    a = None
    gc.collect()
`);
console.log("(outside Python)");

console.log("test that finalize does not get collected before its ref does");
mp.runPython(`
    a = A()
    weakref.finalize(a, callback)
    gc.collect()
`);
console.log("(outside Python)");
mp.runPython(`
    print("free a")
    a = None
    gc.collect()
`);
console.log("(outside Python)");
