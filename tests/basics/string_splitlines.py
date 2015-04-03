try:
    str.splitlines
except:
    import sys
    print("SKIP")
    sys.exit()

print("foo\nbar".splitlines())
print("foo\nbar\n".splitlines())
print("foo\nbar".splitlines(True))
print("foo\nbar\n".splitlines(True))
print("foo\nbar".splitlines(keepends=True))
print("foo\nbar\n".splitlines(keepends=True))
