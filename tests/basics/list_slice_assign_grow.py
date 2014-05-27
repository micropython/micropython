x = list(range(2))

l = list(x)
l[0:0] = [10]
print(l)
l = list(x)
l[:0] = [10, 20]
print(l)
l = list(x)
l[0:0] = [10, 20, 30, 40]
print(l)

l = list(x)
l[1:1] = [10, 20, 30, 40]
print(l)

l = list(x)
l[2:] = [10, 20, 30, 40]
print(l)

# Weird cases
l = list(x)
l[1:0] = [10, 20, 30, 40]
print(l)

l = list(x)
l[100:100] = [10, 20, 30, 40]
print(l)
