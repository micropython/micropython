try:
    import ujson as json
except ImportError:
    try:
        import json
    except ImportError:
        print("SKIP")
        raise SystemExit

print(json.dumps(False))
print(json.dumps(True))
print(json.dumps(None))
print(json.dumps(1))
print(json.dumps('abc'))
print(json.dumps('\x00\x01\x7e'))
print(json.dumps([]))
print(json.dumps([1]))
print(json.dumps([1, 2]))
print(json.dumps([1, True]))
print(json.dumps(()))
print(json.dumps((1,)))
print(json.dumps((1, 2)))
print(json.dumps((1, (2, 3))))
print(json.dumps({}))
print(json.dumps({"a":1}))
print(json.dumps({"a":(2,[3,None])}))
print(json.dumps('"quoted"'))
print(json.dumps('space\n\r\tspace'))
print(json.dumps({None: -1}))
print(json.dumps({False: 0}))
print(json.dumps({True: 1}))
print(json.dumps({1: 2}))
