@micropython.native
def in_set(c):
    z = 0
    for i in range(40):
        z = z*z + c
        if abs(z) > 60:
            return False
    return True

for v in range(31):
    line = []
    for u in range(91):
        line.append('*' if in_set((u / 30 - 2) + (v / 15 - 1) * 1j) else ' ')
    print(''.join(line))
