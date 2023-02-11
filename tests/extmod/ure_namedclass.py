# test named char classes

try:
    import ure as re
except ImportError:
    try:
        import re
    except ImportError:
        print("SKIP")
        raise SystemExit


def print_groups(match):
    print("----")
    try:
        i = 0
        while True:
            print(match.group(i))
            i += 1
    except IndexError:
        pass


m = re.match(r"\w+", "1234hello567 abc")
print_groups(m)

m = re.match(r"(\w+)\s+(\w+)", "ABC \t1234hello567 abc")
print_groups(m)

m = re.match(r"(\S+)\s+(\D+)", "ABC \thello abc567 abc")
print_groups(m)

m = re.match(r"(([0-9]*)([a-z]*)\d*)", "1234hello567")
print_groups(m)

# named class within a class set
print_groups(re.match("([^\s]+)\s*([^\s]+)", "1 23"))
print_groups(re.match("([\s\d]+)([\W]+)", "1  2-+="))
print_groups(re.match("([\W]+)([^\W]+)([^\S]+)([^\D]+)", " a_1 23"))
