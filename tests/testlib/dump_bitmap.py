palette = list("█▓▓▒▒░░·")


def dump_bitmap(b, shifts=(0,)):
    for i in range(b.height):
        for shift in shifts:
            for j in range(b.width):
                # Bit order is gggBBBBBRRRRRGGG" so shift of 0 takes high order bits of G
                p = (b[j, i] >> shift) & 7
                print(end=palette[p])
            print(end=" ")
        print()
    print()


def dump_bitmap_rgb_swapped(b):
    dump_bitmap(b, (5, 0, 10))
