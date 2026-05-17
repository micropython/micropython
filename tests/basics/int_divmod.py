# test integer floor division and modulo

# test all combination of +/-/0 cases
for i in range(-2, 3):
    for j in range(-4, 5):
        if j != 0:
            print(i, j, i // j, i % j, divmod(i, j))
