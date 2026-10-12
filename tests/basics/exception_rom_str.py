# test exceptions with a str argument that is stored in ROM, like sys.version
# (accessing the exception's message must not try to modify the str object)

import sys

try:
    sys.version
except AttributeError:
    print("SKIP")
    raise SystemExit

s = sys.version

e = Exception(s)
print(str(e) == s)
print(e.args == (s,))

try:
    raise ValueError(s)
except ValueError as e:
    print(str(e) == s)
    print(e.args[0] == s)
    # the str object can still be hashed and used as a dict key
    print({s: 1}[e.args[0]])
