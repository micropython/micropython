// Test runPythonAsync() and top-level await in Python, with multi-level awaits.

const mp = await (await import(process.argv[2])).loadMicroPython();

// simulate a 2-step resolution of the string "OK".
await mp.runPythonAsync(`
import js

def _timeout(resolve, _):
    js.setTimeout(resolve, 100)

def _fetch():
    return js.Promise.new(_timeout)

async def _text(promise):
    if not promise._response:
        print("_text await start")
        await promise
        print("_text awaited end")
    ret = await promise._response.text()
    return ret

class _Response:
    async def text(self):
        print('_Response.text start')
        await js.Promise.new(_timeout)
        print('_Response.text end')
        return "OK"

def _response(promise):
    promise._response = _Response()
    return promise._response

def fetch(url):
    promise = _fetch().then(lambda *_: _response(promise))
    promise._response = None
    promise.text = lambda: _text(promise)
    return promise

print(await fetch("config.json").text())
print(await (await fetch("config.json")).text())
`);
