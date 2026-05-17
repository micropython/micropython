try:
    Exception.__class__
except AttributeError:
    print("SKIP")
    raise SystemExit

import sys, pkg

# Modules we import are usually added to sys.modules.
print("pkg" in sys.modules)

try:
    from broken.zerodiv import x
except Exception as e:
    print(e.__class__.__name__)

# The broken module we tried to import should not be in sys.modules.
print("broken.zerodiv" in sys.modules)

# If we try to import the module again, the code should
# run again and we should get the same error.
try:
    from broken.zerodiv import x
except Exception as e:
    print(e.__class__.__name__)

# Import a module that successfully imports some other modules
# before importing the problematic module.
try:
    import broken.pkg2_and_zerodiv
except ZeroDivisionError:
    pass

print("pkg2" in sys.modules)
print("pkg2.mod1" in sys.modules)
print("pkg2.mod2" in sys.modules)
print("broken.zerodiv" in sys.modules)
print("broken.pkg2_and_zerodiv" in sys.modules)
