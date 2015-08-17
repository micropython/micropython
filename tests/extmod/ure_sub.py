try:
    import ure as re
except ImportError:
    import re


def multiply(m):
    v = int(m.group(0))
    return str(v * 2)

result = re.sub("\d+", multiply, "10 20 30 40 50")
print(result)

result = re.sub("\d+", lambda m: str(int(m.group(0)) * 2), "10 20 30 40 50")
print(result)

print(re.sub('a', 'A', 'aa678gjhg989000asadsadsasa'))
print(re.sub('a', 'A', 'ab'))

print(
    re.sub(
        r'def\s+([a-zA-Z_][a-zA-Z_0-9]*)\s*\(\s*\):', 
        'static PyObject*\npy_\\1(void){\n  return;\n}\n', 
        'AAAdef myfunc():AAAA\ndef myfunc1():AAAAA\ndef myfunc2():\n'
    )
) 

where = 'abcdefghijklmnopqrstuvwxyz' + '0123456789' + 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
replacement = "".join( [ 
        (
            "\\g<{0}>" if r%2 else "\\{0}"
        ).format(r) for r in range(1, len(where) + 1)
    ][::-1] 
) 
regex = "([a-zA-Z0-9])" * len(where)

s0 = re.sub(regex, replacement, where)
print(s0, len(s0)) 

if True:
    
    regex = r'(calzino)(\ )(blu|bianco|rossa).*(scarpa).*(blu|bianco|rossa)'
    replacement = r'\g<1> \2colore (\3) ma veramente tanto (\g<3>) \g<4> colore (\5) ma veramente tanto (\g<5>)'
    where = 'calzino blu e scarpa rossa!'
    
    for index in range(1):

        p1 = re.compile(regex)
        s1 = p1.sub(replacement, where)
        print(s1, len(s1))

        s2 = re.sub(regex, replacement, where)
        print(s2, len(s2))

        p3 = re.compile(regex)
        s3 = p3.sub(replacement, where, 1)
        print(s3, len(s3))

        def callable(value):
            return value

        s4 = re.sub(
            regex, 
            callable(replacement), 
            where
        )
        print(s4, len(s4))

        p5 = re.compile(regex)
        s5 = p5.sub(
            callable(replacement), 
            where, 
            1
        )
        print(s5, len(s5))
