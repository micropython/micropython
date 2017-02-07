try:
    from uio import StringIO
    import ujson as json
except:
    from io import StringIO
    import json

print(json.load(StringIO('null')))
print(json.load(StringIO('"abc\\u0064e"')))
print(json.load(StringIO('[false, true, 1, -2]')))
print(json.load(StringIO('{"a":true}')))
