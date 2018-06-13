try:
    import ure as re
except ImportError:
    import re

try:
    re.sub
except AttributeError:
    print('SKIP')
    raise SystemExit


def multiply(m):
    return str(int(m.group(0)) * 2)

print(re.sub("\d+", multiply, "10 20 30 40 50"))

print(re.sub("\d+", lambda m: str(int(m.group(0)) // 2), "10 20 30 40 50"))

def A():
    return "A"
print(re.sub('a', A(), 'aBCBABCDabcda.'))

print(
    re.sub(
        r'def\s+([a-zA-Z_][a-zA-Z_0-9]*)\s*\(\s*\):', 
        'static PyObject*\npy_\\1(void){\n  return;\n}\n', 
        '\n\ndef myfunc():\n\ndef myfunc1():\n\ndef myfunc2():'
    )
) 

print(
    re.compile(
         '(calzino) (blu|bianco|verde) e (scarpa) (blu|bianco|verde)'
    ).sub(
        r'\g<1> colore \2 con \g<3> colore \4? ...',
        'calzino blu e scarpa verde'
    )
)

# no matches at all
print(re.sub('a', 'b', 'c'))

# first group matches, second optional group doesn't so is replaced with a blank
print(re.sub(r'(a)(b)?', r'\2-\1', '1a2'))

# with maximum substitution count specified
print(re.sub('a', 'b', '1a2a3a', 2))
