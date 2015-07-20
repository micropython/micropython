try:
    import ure as re
except ImportError:
    import re

s0 = re.sub(r'def\s+([a-zA-Z_][a-zA-Z_0-9]*)\s*\(\s*\):',
       'static PyObject*\npy_\1(void)\n{',
       'def myfunc():')
print(s0)

regex = r'(.*)(blu|bianco|verde)(.*)(blu|bianco|verde)$'
replacement = r'\1colore (\2) ma veramente tanto(\2)\3colore (\4) ma veramente tanto (\4)'
where = 'calzino blu e scarpa verde'
p1 = re.compile(regex)
s1 = p1.sub(replacement, where)
print(s1)

s2 = re.sub(regex, replacement, where)
print(s2)

p3 = re.compile(regex)
s3 = p3.sub(replacement, where, 1)
print(s3)

def callable(value):
    return value

s4 = re.sub(
    regex, 
    callable(replacement), 
    where
)
print(s4)

p5 = re.compile(regex)
s5 = p5.sub(
    callable(replacement), 
    where, 
    1
)
print(s5)
