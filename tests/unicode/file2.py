# test reading a given number of characters

def do(mode):
    if mode == 'rb':
        enc = None
    else:
        enc = 'utf-8'
    f = open('unicode/data/utf-8_2.txt', mode=mode, encoding=enc)
    print(f.read(1))
    print(f.read(1))
    print(f.read(2))
    print(f.read(4))

    # skip to end of line
    f.readline()

    # check 3-byte utf-8 char
    print(f.read(1 if mode == 'rt' else 3))

    # check 4-byte utf-8 char
    print(f.read(1 if mode == 'rt' else 4))

    f.close()

do('rb')
do('rt')
