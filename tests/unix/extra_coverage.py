try:
    extra_coverage
except NameError:
    print("SKIP")
    import sys
    sys.exit()

data = extra_coverage()

# test hashing of str/bytes that have an invalid hash
print(data)
print(hash(data[0]))
print(hash(data[1]))
print(hash(bytes(data[0], 'utf8')))
print(hash(str(data[1], 'utf8')))
