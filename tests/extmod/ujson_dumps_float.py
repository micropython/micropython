try:
    import ujson as json
except ImportError:
    import json

print(json.dumps(1.2))
