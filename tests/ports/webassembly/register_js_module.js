import(process.argv[2]).then((mp) => {
    mp.loadMicroPython().then((py) => {
        py.registerJsModule("js_module", { y: 2 });
        py.runPython("import js_module; print(js_module); print(js_module.y)");
    });
});
