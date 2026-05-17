// Test raising an exception in async Python code running in runPythonAsync,
// that the JavaScript-level promise is rejected with a PythonError.

const mp = await (await import(process.argv[2])).loadMicroPython();

try {
    await mp.runPythonAsync("await fail");
} catch (error) {
    console.log(error.name, error.type);
    console.log(error.message);
}
