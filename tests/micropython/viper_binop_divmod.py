# test floor-division and modulo operators


@micropython.viper
def div(x: int, y: int) -> int:
    return x // y


@micropython.viper
def mod(x: int, y: int) -> int:
    return x % y


def dm(x, y):
    print(div(x, y), mod(x, y))


for x in (-6, 6):
    for y in range(-7, 8):
        if y == 0:
            continue
        dm(x, y)
