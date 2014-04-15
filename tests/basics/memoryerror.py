# this test for MemoryError can be difficult to reproduce
# on different machine configurations (notably Travis CI)
# so we disable it
# TODO is there a way of testing that we are on Travis CI?
if False:
    l = list(range(10000))
    try:
        100000000 * l
    except MemoryError:
        print('MemoryError')
    print(len(l), l[0], l[-1])
