// Test the registerJsModule() public API method.

import(process.argv[2]).then((mp) => {
    mp.loadMicroPython().then((py) => {
        // Simple module.
        py.registerJsModule("js_module", { y: 2 });
        py.runPython("import js_module; print(js_module); print(js_module.y)");

        // Module with functions.
        // In particular test how "this" behaves.
        py.registerJsModule("js_module2", {
            yes: true,
            add1(x) {
                return x + 1;
            },
            getThis() {
                return this;
            },
        });

        console.log("====");

        // Test using simple import.
        py.runPython(`
import js_module2

print(js_module2.yes)
print(js_module2.add1(1))
print(js_module2.getThis())
print(js_module2.getThis().yes)
`);

        console.log("====");

        // Test using "from ... import".
        py.runPython(`
from js_module2 import yes, add1, getThis

print(yes)
print(add1(2))
print(getThis())
print(getThis().yes)
`);
    });
});
