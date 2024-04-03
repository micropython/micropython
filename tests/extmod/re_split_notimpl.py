try:
    import re
except ImportError:
    print("SKIP")
    raise SystemExit

r = re.compile("( )")
try:
    s = r.split("a b c foobar")
except NotImplementedError:
    print("NotImplementedError")
