# test printing debugging info when compiling
try:
    import re

    re.DEBUG
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

re.compile("^a|b[0-9]\w$", re.DEBUG)
