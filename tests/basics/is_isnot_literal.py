# test "is" and "is not" with literal arguments
# these raise a SyntaxWarning in CPython because the results are
# implementation dependent; see https://bugs.python.org/issue34850

print(1 is 1)
print(1 is 2)
print(1 is not 1)
print(1 is not 2)

print("a" is "a")
print("a" is "b")
print("a" is not "a")
print("a" is not "b")
