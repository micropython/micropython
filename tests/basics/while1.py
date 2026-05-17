# basic while loop

x = 0
while x < 2:
    y = 0
    while y < 2:
        z = 0
        while z < 2:
            z = z + 1
            print(x, y, z)
        y = y + 1
    x = x + 1
