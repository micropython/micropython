// Test await'ing on a JavaScript async function that throws a JavaScript Error.

const mp = await (await import(process.argv[2])).loadMicroPython();

globalThis.foo = async () => {
    console.log(2);
    throw Error("test");
};

await mp.runPythonAsync(`
import js, jsffi
print(1)
try:
    await js.foo()
except jsffi.JsException as er:
    error = er
print(error)
print(3)
`);

const error = mp.globals.get("error");
console.log(error instanceof Error, error.name, error.message);
