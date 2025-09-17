# test `from package import *` conventions, including __all__ support
#
# This test requires MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_BASIC_FEATURES

try:
    next(iter([]), 42)
except TypeError:
    # 2-argument version of next() not supported
    # we are probably not at MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES
    print("SKIP")
    raise SystemExit

# 1. test default visibility
from pkgstar_default import *

print("visibleFun" in globals())
print("VisibleClass" in globals())
print("_hiddenFun" in globals())
print("_HiddenClass" in globals())
print(visibleFun())

# 2. test explicit visibility as defined by __all__ (as an array)
from pkgstar_all_array import *

print("publicFun" in globals())
print("PublicClass" in globals())
print("unlistedFun" in globals())
print("UnlistedClass" in globals())
print("_privateFun" in globals())
print("_PrivateClass" in globals())
print(publicFun())
# test dynamic import as used in asyncio
print("dynamicFun" in globals())
print(dynamicFun())

# 3. test explicit visibility as defined by __all__ (as an tuple)
from pkgstar_all_tuple import *

print("publicFun2" in globals())
print("PublicClass2" in globals())
print("unlistedFun2" in globals())
print("UnlistedClass2" in globals())
print(publicFun2())

# 4. test reporting of missing entries in __all__
try:
    from pkgstar_all_miss import *

    print("missed detection of incorrect __all__ definition")
except AttributeError as er:
    print("AttributeError triggered for bad __all__ definition")

# 5. test reporting of invalid __all__ definition
try:
    from pkgstar_all_inval import *

    print("missed detection of incorrect __all__ definition")
except TypeError as er:
    print("TypeError triggered for bad __all__ definition")
