// Test that mp_js_hook calls mp_hal_get_interrupt_char() with the correct ABI
// (no arguments). The hook fires every 10 bytecodes, so a loop of 20 iterations
// is enough to exercise it.

import(process.argv[2]).then((mp) => {
    mp.loadMicroPython().then((py) => {
        py.runPython("for i in range(20): pass");
        console.log("hook ok");
    });
});
