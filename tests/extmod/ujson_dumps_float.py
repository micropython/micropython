try:
    import ujson as json
except ImportError:
    try:
        import json
    except ImportError:
        print("SKIP")
        raise SystemExit

print(json.dumps(1.2))
