# Test builtin slice indices resolution

# A class that returns an item key
class A:
    def __getitem__(self, idx):
        return idx

# Make sure that we have slices and .indices()
try:
    A()[2:5].indices(10)
except:
    print("SKIP")
    raise SystemExit

print(A()[:].indices(10))
print(A()[2:].indices(10))
print(A()[:7].indices(10))
print(A()[2:7].indices(10))
print(A()[2:7:2].indices(10))
print(A()[2:7:-2].indices(10))
print(A()[7:2:2].indices(10))
print(A()[7:2:-2].indices(10))

print(A()[2:7:2].indices(5))
print(A()[2:7:-2].indices(5))
print(A()[7:2:2].indices(5))
print(A()[7:2:-2].indices(5))
