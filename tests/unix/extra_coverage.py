try:
    extra_coverage
except NameError:
    print("SKIP")
    import sys
    sys.exit()

data = extra_coverage()

# test hashing of str/bytes that have an invalid hash
print(data)
print(hash(data[0]))
print(hash(data[1]))
print(hash(bytes(data[0], 'utf8')))
print(hash(str(data[1], 'utf8')))

# test basic import of frozen scripts
import frzstr1
import frzmpy1

# test import of frozen packages with __init__.py
import frzstr_pkg1
print(frzstr_pkg1.x)
import frzmpy_pkg1
print(frzmpy_pkg1.x)

# test import of frozen packages without __init__.py
from frzstr_pkg2.mod import Foo
print(Foo.x)
from frzmpy_pkg2.mod import Foo
print(Foo.x)
