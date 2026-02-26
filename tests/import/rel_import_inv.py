try:
    from . import foo
except Exception:
    print("Invalid relative import caught")
