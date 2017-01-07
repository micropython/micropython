# set type

# This doesn't really work as expected, because {None}
# leads SyntaxError during parsing.
try:
    set
except NameError:
    import sys
    print("SKIP")
    sys.exit()

print(set)

print(type(set()) == set)

print(type({None}) == set)
