# set type

# This doesn't really work as expected, because {None}
# leads SyntaxError during parsing.
try:
    set
except NameError:
    print("SKIP")
    raise SystemExit

print(set)

print(type(set()) == set)

print(type({None}) == set)
