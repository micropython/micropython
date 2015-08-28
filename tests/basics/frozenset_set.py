try:
    frozenset
except NameError:
    print("SKIP")
    import sys
    sys.exit()

# Examples from https://docs.python.org/3/library/stdtypes.html#set
# "Instances of set are compared to instances of frozenset based on their
# members. For example:"
print(set('abc') == frozenset('abc'))
# This doesn't work in uPy
#print(set('abc') in set([frozenset('abc')]))
