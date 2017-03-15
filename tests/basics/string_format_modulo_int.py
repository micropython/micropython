# test string modulo formatting with int values

# test + option with various amount of padding
for pad in ('', ' ', '0'):
    for n in (1, 2, 3):
        for val in (-1, 0, 1):
            print(('%+' + pad + str(n) + 'd') % val)
