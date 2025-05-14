# test match.span() for unicode strings

try:
    import re
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    m = re.match(".", "a")
    m.span
except AttributeError:
    print("SKIP")
    raise SystemExit


def print_spans(match):
    print("----")
    try:
        i = 0
        while True:
            print(match.span(i), match.start(i), match.end(i))
            i += 1
    except IndexError:
        pass


m = re.match(r"([0-9]*)(([a-z]*)([0-9]*))", "1234\u2764567")
print_spans(m)
