# test builtin slice

# ensures that slices passed to user types are heap-allocated and can be
# safely stored as well as not overriden by subsequent slices.

# print slice
class A:
    def __getitem__(self, idx):
        print("get", idx)
        print("abc"[1:])
        print("get", idx)
        return idx

    def __setitem__(self, idx, value):
        print("set", idx)
        print("abc"[1:])
        print("set", idx)
        self.saved_idx = idx
        return idx

    def __delitem__(self, idx):
        print("del", idx)
        print("abc"[1:])
        print("del", idx)
        return idx


a = A()
s = a[1:2:3]
a[4:5:6] = s
del a[7:8:9]

print(a.saved_idx)

# nested slicing
print(A()[1 : A()[A()[2:3:4] : 5]])

# tuple slicing
a[1:2, 4:5, 7:8]
a[1, 4:5, 7:8, 2]
a[1:2, a[3:4], 5:6]

# check type
print(type(s) is slice)
