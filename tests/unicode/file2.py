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
    f.close()

do('rb')
do('rt')
