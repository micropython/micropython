try:
    import ure as re
except ImportError:
    try:
        import re
    except ImportError:
        print("SKIP")
        raise SystemExit

try:
    re.sub
except AttributeError:
    print("SKIP")
    raise SystemExit


def multiply(m):
    return str(int(m.group(0)) * 2)


print(re.sub("\d+", multiply, "10 20 30 40 50"))

print(re.sub("\d+", lambda m: str(int(m.group(0)) // 2), "10 20 30 40 50"))


def A():
    return "A"


print(re.sub("a", A(), "aBCBABCDabcda."))

print(
    re.sub(
        r"def\s+([a-zA-Z_][a-zA-Z_0-9]*)\s*\(\s*\):",
        "static PyObject*\npy_\\1(void){\n  return;\n}\n",
        "\n\ndef myfunc():\n\ndef myfunc1():\n\ndef myfunc2():",
    )
)

print(
    re.compile("(calzino) (blu|bianco|verde) e (scarpa) (blu|bianco|verde)").sub(
        r"\g<1> colore \2 con \g<3> colore \4? ...", "calzino blu e scarpa verde"
    )
)

# \g immediately followed by another \g
print(re.sub("(abc)", r"\g<1>\g<1>", "abc"))

# no matches at all
print(re.sub("a", "b", "c"))

# with maximum substitution count specified
print(re.sub("a", "b", "1a2a3a", 2))

# invalid group
try:
    re.sub("(a)", "b\\2", "a")
except:
    print("invalid group")

# invalid group with very large number (to test overflow in uPy)
try:
    re.sub("(a)", "b\\199999999999999999999999999999999999999", "a")
except:
    print("invalid group")

# Module function takes str/bytes/re.
print(re.sub("a", "a", "a"))
print(re.sub(b".", b"a", b"a"))
print(re.sub(re.compile("a"), "a", "a"))
try:
    re.sub(123, "a", "a")
except TypeError:
    print("TypeError")

# Include \ in the sub replacement
print(re.sub("b", "\\\\b", "abc"))

# Using ^, make sure it doesn't repeatedly match
print(re.sub("^ab", "*", "abababcabab"))
print(re.sub("^ab|cab", "*", "abababcabab"))
