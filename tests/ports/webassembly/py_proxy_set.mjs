// Test `<py-obj>.<attr> = <value>` on the JavaScript side, which tests PyProxy.set.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
class A:
    pass
x = A()
y = []
`);

const x = mp.globals.get("x");
const y = mp.globals.get("y");

// Should pass.
x.foo = 1;

mp.runPython(`
print(x.foo)
`);

// Should fail, can't set attributes on MicroPython lists.
try {
    y.bar = 1;
} catch (error) {
    console.log(error.message);
}
