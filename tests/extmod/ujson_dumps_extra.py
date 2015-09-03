# test uPy ujson behaviour that's not valid in CPy

import ujson

print(ujson.dumps(b'1234'))
