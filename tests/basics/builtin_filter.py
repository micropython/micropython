try:
    filter
except:
    print("SKIP")
    raise SystemExit

print(list(filter(lambda x: x & 1, range(-3, 4))))
print(list(filter(None, range(-3, 4))))
