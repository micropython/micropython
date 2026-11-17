// Test awaiting something in runPythonAsync that is not a JavaScript
// promise: the error raised while resuming the generator must reject the
// JavaScript-level promise rather than escape as an uncaught error.

const mp = await (await import(process.argv[2])).loadMicroPython();

const result = await mp
    .runPythonAsync(`
class A:
    def __iter__(self):
        yield 0
await A()
`)
    .then(
        () => "resolved",
        (error) => `rejected: ${error.name} ${error.type}`,
    );
console.log(result);
