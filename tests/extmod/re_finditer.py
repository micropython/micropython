try:
    import re
    from re import finditer
except ImportError:
    print("SKIP")
    raise SystemExit

ms = re.finditer(r"f[a-z]*", "which foot or hand fell fastest")
print(list(x.group(0) for x in ms))

p = re.compile(r"f[a-z]*")
ms = p.finditer("which foot or hand fell fastest")
print(list(x.group(0) for x in ms))

ms = p.finditer("which foot or hand fell fastest", 10)
print(list(x.group(0) for x in ms))

ms = p.finditer("which foot or hand fell fastest", 10, 21)
print(list(x.group(0) for x in ms))

ms = re.finditer(r"\s+", "which foot or hand fell fastest")
print(list(x.group(0) for x in ms))

ms = re.finditer(r"zz", "which foot or hand fell fastest")
print(list(x.group(0) for x in ms))
