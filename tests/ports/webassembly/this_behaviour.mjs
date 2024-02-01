// Test "this" behaviour.

const mp = await (await import(process.argv[2])).loadMicroPython();

// "this" should be undefined.
globalThis.func0 = function () {
    console.log("func0", this);
};
mp.runPython("import js; js.func0()");

globalThis.func1 = function (a) {
    console.log("func1", a, this);
};
mp.runPython("import js; js.func1(123)");

globalThis.func2 = function (a, b) {
    console.log("func2", a, b, this);
};
mp.runPython("import js; js.func2(123, 456)");

globalThis.func3 = function (a, b, c) {
    console.log("func3", a, b, c, this);
};
mp.runPython("import js; js.func3(123, 456, 789)");
