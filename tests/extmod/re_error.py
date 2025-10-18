# test errors in regex

try:
    import re
except ImportError:
    print("SKIP")
    raise SystemExit


def test_re(r):
    try:
        re.compile(r)
        print("OK")
    except:  # MPy and CPy use different errors, so just ignore the type
        print("Error")


test_re(r"?")
test_re(r"*")
test_re(r"+")
test_re(r")")
test_re(r"[")
test_re(r"([")
test_re(r"([)")
test_re(r"[a\]")
