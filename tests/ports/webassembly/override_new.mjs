// Test overriding .new() on a JavaScript class.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.MyClass1 = class {
    new() {
        console.log("MyClass1 new");
        return 1;
    }
};

globalThis.MyClass2 = class {
    static new() {
        console.log("MyClass2 static new");
        return 2;
    }
    new() {
        console.log("MyClass2 new");
        return 3;
    }
};

globalThis.myClass2Instance = new globalThis.MyClass2();

mp.runPython(`
    import js

    print(type(js.MyClass1.new()))
    print(js.MyClass1.new().new())

    print(js.MyClass2.new())
    print(js.myClass2Instance.new())
`);
