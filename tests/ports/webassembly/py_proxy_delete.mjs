// Test `delete <py-obj>.<attr>` on the JavaScript side, which tests PyProxy.deleteProperty.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
class A:
    pass
x = A()
x.foo = 1
y = []
`);

const x = mp.globals.get("x");
const y = mp.globals.get("y");

// Should pass.
// biome-ignore lint/performance/noDelete: test delete statement
delete x.foo;

mp.runPython(`
print(hasattr(x, "foo"))
`);

// Should fail, can't delete attributes on MicroPython lists.
try {
    // biome-ignore lint/performance/noDelete: test delete statement
    delete y.sort;
} catch (error) {
    console.log(error.message);
}
