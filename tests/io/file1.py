f = open("io/data/file1")
print(f.read(5))
print(f.readline())
print(f.read())
f = open("io/data/file1")
print(f.readlines())
f = open("io/data/file1","r")
print(f.readlines())
f = open("io/data/file1","rb")
print(f.readlines())
f = open("io/data/file1",mode="r")
print(f.readlines())
f = open("io/data/file1",mode="rb")
print(f.readlines())

# write() error
f = open('io/data/file1', 'r')
try:
    f.write('x')
except OSError:
    print('OSError')
f.close()

# read(n) error on binary file
f = open('io/data/file1', 'ab')
try:
    f.read(1)
except OSError:
    print('OSError')
f.close()

# read(n) error on text file
f = open('io/data/file1', 'at')
try:
    f.read(1)
except OSError:
    print('OSError')
f.close()

# readall() error (call read() for compat with CPy)
f = open('io/data/file1', 'ab')
try:
    f.read()
except OSError:
    print('OSError')
f.close()
