try:
    zip
    set
except NameError:
    print("SKIP")
    import sys
    sys.exit()

print(list(zip()))
print(list(zip([1], set([2, 3]))))
