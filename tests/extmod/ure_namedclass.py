# test named char classes

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

m = re.match(r'\w+','1234hello567 abc')
print_groups(m)

m = re.match(r'(\w+)\s+(\w+)','ABC \t1234hello567 abc')
print_groups(m)

m = re.match(r'(\S+)\s+(\D+)','ABC \thello abc567 abc')
print_groups(m)

m = re.match(r'(([0-9]*)([a-z]*)\d*)','1234hello567')
print_groups(m)
