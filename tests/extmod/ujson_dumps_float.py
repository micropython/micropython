try:
    import ujson as json
except ImportError:
    try:
        import json
    except ImportError:
        import sys
        print("SKIP")
        sys.exit()

print(json.dumps(1.2))
