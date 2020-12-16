# copying a large dictionary

a = {i:2*i for i in range(1000)}
b = a.copy()
for i in range(1000):
    print(i, b[i])
print(len(b))
