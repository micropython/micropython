a = {i: 2*i for i in range(100)}
b = a.copy()
for i in range(100):
    print(i, b[i])
print(len(b))
