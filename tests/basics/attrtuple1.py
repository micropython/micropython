# test attrtuple
# we can't test this type directly so we use sys.implementation object

import sys
t = sys.implementation

# It can be just a normal tuple on small ports
try:
    t.name
except AttributeError:
    import sys
    print("SKIP")
    sys.exit()


# test printing of attrtuple
print(str(t).find("version=") > 0)

# test read attr
print(isinstance(t.name, str))
