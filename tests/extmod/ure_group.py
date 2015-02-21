# test groups, and nested groups

try:
    import ure as re
except:
    import re

def print_groups(match):
    print('----')
    try:
        i = 0
        while True:
            print(m.group(i))
            i += 1
    except IndexError:
        pass

m = re.match(r'(([0-9]*)([a-z]*)[0-9]*)','1234hello567')
print_groups(m)

m = re.match(r'([0-9]*)(([a-z]*)([0-9]*))','1234hello567')
print_groups(m)
