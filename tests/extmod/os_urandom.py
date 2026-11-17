# Test os.urandom().

try:
    from os import urandom
except ImportError:
    print("SKIP")
    raise SystemExit

for n in range(-2, 5, 1):
    try:
        r = urandom(n)
        print(n, type(r), len(r))
    except ValueError:
        print(n, "ValueError")
