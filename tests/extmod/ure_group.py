# test groups, and nested groups

try:
    import ure as re
except ImportError:
    try:
        import re
    except ImportError:
        import sys
        print("SKIP")
        sys.exit()

def print_groups(match):
    print('----')
    try:
        i = 0
        while True:
            print(match.group(i))
            i += 1
    except IndexError:
        pass

m = re.match(r'(([0-9]*)([a-z]*)[0-9]*)','1234hello567')
print_groups(m)

m = re.match(r'([0-9]*)(([a-z]*)([0-9]*))','1234hello567')
print_groups(m)

# optional group that matches
print_groups(re.match(r'(a)?b(c)', 'abc'))

# optional group that doesn't match
print_groups(re.match(r'(a)?b(c)', 'bc'))
