try:
    import ure as re
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

r = re.compile('( )')
try:
    s = r.split("a b c foobar")
except NotImplementedError:
    print('NotImplementedError')
