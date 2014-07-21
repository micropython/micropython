# test reading a given number of characters

def do(mode):
    f = open('unicode/data/utf-8_2.txt', mode)
    print(f.read(1))
    print(f.read(1))
    print(f.read(2))
    print(f.read(4))
    f.close()

do('rb')
do('rt')
