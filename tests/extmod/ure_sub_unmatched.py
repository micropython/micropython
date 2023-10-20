# test re.sub with unmatched groups, behaviour changed in CPython 3.5

try:
    import re
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    re.sub
except AttributeError:
    print("SKIP")
    raise SystemExit

# first group matches, second optional group doesn't so is replaced with a blank
print(re.sub(r"(a)(b)?", r"\2-\1", "1a2"))
