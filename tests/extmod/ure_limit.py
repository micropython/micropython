try:
    import ure as re
except ImportError:
    print("SKIP")
    raise SystemExit


def test_re(r):
    try:
        re.compile(r)
    except:
        print("Error")


test_re("b" * 63 + "|a")
