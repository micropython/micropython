try:
    import json
    from collections import OrderedDict
except ImportError:
    print("SKIP")
    raise SystemExit

print(json.dumps(OrderedDict(((1, 2), (3, 4)))))
