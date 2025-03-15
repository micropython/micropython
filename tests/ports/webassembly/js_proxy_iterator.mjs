// Test accessing JavaScript iterables (objects with Symbol.iterator) from Python.

const mp = await (await import(process.argv[2])).loadMicroPython();

mp.runPython(`
import js

for v in js.Set.new([1, 2]):
    print(v)

url_search_params = js.URLSearchParams.new("one=1&two=2")
for key in url_search_params.keys():
    print(key, list(url_search_params.getAll(key)))
`);
