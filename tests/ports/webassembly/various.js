import(process.argv[2]).then((mp) => {
    mp.loadMicroPython().then((py) => {
        globalThis.jsadd = (x, y) => {
            return x + y;
        };
        py.runPython("import js; print(js); print(js.jsadd(4, 9))");

        py.runPython(
            "def set_timeout_callback():\n print('set_timeout_callback')",
        );
        py.runPython("import js; js.setTimeout(set_timeout_callback, 100)");

        py.runPython("obj = js.Object(a=1)");
        console.log("main", py.pyimport("__main__").obj);

        console.log("=======");
        py.runPython(`
            from js import Array, Promise, Reflect

            def callback(resolve, reject):
                resolve('OK1')

            p = Reflect.construct(Promise, Array(callback))
            p.then(print)
        `);

        console.log("=======");
        py.runPython(`
            from js import Promise

            def callback(resolve, reject):
                resolve('OK2')

            p = Promise.new(callback)
            p.then(print)
        `);
    });
});
