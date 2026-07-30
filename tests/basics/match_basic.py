# Experimental match/case subset (MICROPY_PY_MATCH)

def test_literal():
    def f(x):
        match x:
            case True:
                return "true"
            case None:
                return "none"
            case 1:
                return "one"
            case "a":
                return "str"
            case _:
                return "other"

    assert f(True) == "true"
    assert f(None) == "none"
    assert f(1) == "one"
    assert f("a") == "str"
    assert f(9) == "other"


def test_capture():
    def f(x):
        match x:
            case v:
                return v

    assert f(42) == 42


def test_or_and_guard():
    def f(x):
        match x:
            case 1 | 2 | 3 if x != 2:
                return "ok"
            case _:
                return "no"

    assert f(1) == "ok"
    assert f(2) == "no"
    assert f(3) == "ok"
    assert f(4) == "no"


def test_sequence():
    def f(x):
        match x:
            case (1, y):
                return ("t", y)
            case [a, b]:
                return ("l", a, b)
            case ():
                return "empty"
            case _:
                return "no"

    assert f((1, "z")) == ("t", "z")
    assert f([7, 8]) == ("l", 7, 8)
    assert f(()) == "empty"
    assert f((1,)) == "no"


test_literal()
test_capture()
test_or_and_guard()
test_sequence()
print("PASS")
