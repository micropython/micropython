# test builtin min and max functions with float args
try:
    min
    max
except:
    print("SKIP")
    raise SystemExit

print(min(0, 1.0))
print(min(1.0, 0))
print(min(0, -1.0))
print(min(-1.0, 0))

print(max(0, 1.0))
print(max(1.0, 0))
print(max(0, -1.0))
print(max(-1.0, 0))

print(min(1.5, -1.5))
print(min(-1.5, 1.5))

print(max(1.5, -1.5))
print(max(-1.5, 1.5))

print(min([1, 2.9, 4, 0, -1, 2]))
print(max([1, 2.9, 4, 0, -1, 2]))

print(min([1, 2.9, 4, 6.5, -1, 2]))
print(max([1, 2.9, 4, 6.5, -1, 2]))
print(min([1, 2.9, 4, -6.5, -1, 2]))
print(max([1, 2.9, 4, -6.5, -1, 2]))

