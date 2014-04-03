f = open("io/data/file1")

with f as f2:
    print(f2.read())

# File should be closed
try:
    f.read()
except:
    # Note: CPython and us throw different exception trying to read from
    # close file.
    print("can't read file after with")
