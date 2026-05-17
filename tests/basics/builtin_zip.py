try:
    zip
    set
except NameError:
    print("SKIP")
    raise SystemExit

print(list(zip()))
print(list(zip([1], set([2, 3]))))
