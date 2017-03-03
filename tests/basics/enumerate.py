try:
    enumerate
except:
    import sys
    print("SKIP")
    sys.exit()

print(list(enumerate([])))
print(list(enumerate([1, 2, 3])))
print(list(enumerate([1, 2, 3], 5)))
print(list(enumerate([1, 2, 3], -5)))
print(list(enumerate(range(100))))

# specifying args with keywords
print(list(enumerate([1, 2, 3], start=1)))
print(list(enumerate(iterable=[1, 2, 3])))
print(list(enumerate(iterable=[1, 2, 3], start=1)))
