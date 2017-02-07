import ure as re

r = re.compile('( )')
try:
    s = r.split("a b c foobar")
except NotImplementedError:
    print('NotImplementedError')
