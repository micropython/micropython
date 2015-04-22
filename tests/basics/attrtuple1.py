# test attrtuple
# we can't test this type directly so we use sys.implementation object

import sys
t = sys.implementation

# test printing of attrtuple
print(str(t).find("version=") > 0)

# test read attr
print(isinstance(t.name, str))
