import(process.argv[2]).then((mp) => {
    mp.loadMicroPython().then((py) => {
        py.runPython("1");
        py.runPython("print('hello')");
        py.runPython("import sys; print(f'hello from {sys.platform}')");
        py.runPython("import collections; print(collections.OrderedDict)");
    });
});
