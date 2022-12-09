def run_test(f, k=None):
    try:
        v = getenv_from_file(f"{BASE}/{f}.toml", k or f)
        print(f, k, repr(v))
    except Exception as e:
        print(f, k, str(e))


BASE = __file__.rpartition("/")[0] or "."

run_test("good", "notpresent")
run_test("good", "string")
run_test("good", "number")
run_test("good", "cstring")
run_test("good", "cnumber")
run_test("good", "subvalue")
for i in range(8):
    run_test("good", f"string{i}")
for i in range(1, 5):
    run_test(f"bad{i}", f"string")
