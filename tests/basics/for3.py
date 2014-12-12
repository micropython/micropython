# test assigning to iterator within the loop
for i in range(2):
    print(i)
    i = 2

# test assigning to range parameter within the loop
# (since we optimise for loops, this needs checking, currently it fails)
n = 2
for i in range(n):
    print(i)
    n = 0
