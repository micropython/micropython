import os

try:
    from _environ import get_key
except:
    # Because run-tests.py suppresses site-packages, this test can't be run
    # on the host interpreter. However, it can be run manually to
    # generate/update the expected file.
    #
    # After 3.11 becomes standard, change this to use tomllib.
    import tomlkit

    def get_key(filename, key):
        with open(filename) as f:
            s = tomlkit.load(f)
            return s.get(key, None)


def run_test(f, k=None):
    try:
        v = get_key(f"{BASE}/{f}.toml", k or f)
        print(f, k, repr(v))
    except Exception as e:
        print(f, k, "err")


if "/" in __file__:
    BASE = __file__.rsplit("/", 1)[0]
else:
    BASE = "."

run_test("good", "notpresent")
run_test("good", "string")
run_test("good", "number")
run_test("good", "cstring")
run_test("good", "cnumber")
run_test("good", "subvalue")
for i in range(8):
    run_test("good", f"string{i}")
run_test("bad1", "string")
run_test("bad2", "string")
run_test("bad3", "string")
