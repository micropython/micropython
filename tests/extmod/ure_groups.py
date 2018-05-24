# test groups

try:
    import ure as re
except ImportError:
    try:
        import re
    except ImportError:
        print("SKIP")
        raise SystemExit

try:
    m = re.match(".", "a")
    m.groups
except AttributeError:
    print('SKIP')
    raise SystemExit


def print_groups(match):
    print('----')
    print(match.groups())


m = re.match(r'(([0-9]*)([a-z]*)[0-9]*)','1234hello567')
print_groups(m)

m = re.match(r'([0-9]*)(([a-z]*)([0-9]*))','1234hello567')
print_groups(m)

# optional group that matches
print_groups(re.match(r'(a)?b(c)', 'abc'))

# optional group that doesn't match
print_groups(re.match(r'(a)?b(c)', 'bc'))
