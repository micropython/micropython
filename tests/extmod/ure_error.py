# test errors in regex

try:
    import ure as re
except:
    import re

def test_re(r):
    try:
        re.compile(r)
        print("OK")
    except: # uPy and CPy use different errors, so just ignore the type
        print("Error")

test_re(r'?')
test_re(r'*')
test_re(r'+')
test_re(r')')
test_re(r'[')
test_re(r'([')
test_re(r'([)')
