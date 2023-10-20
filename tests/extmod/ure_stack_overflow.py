try:
    import re
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    re.match("(a*)*", "aaa")
except RuntimeError:
    print("RuntimeError")
