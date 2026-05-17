// Test polyfill of a method on a built-in.

// Implement Promise.withResolvers, and make sure it has a unique name so
// the test below is guaranteed to use this version.
Promise.withResolversCustom = function withResolversCustom() {
    let a;
    let b;
    const c = new this((resolve, reject) => {
        a = resolve;
        b = reject;
    });
    return { resolve: a, reject: b, promise: c };
};

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
    from js import Promise

    deferred = Promise.withResolversCustom()
    deferred.promise.then(print)
    deferred.resolve('OK')
`);
