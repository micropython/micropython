# Test that __file__ is set correctly for script execution
try:
    print("__file__ =", __file__)
except NameError:
    print("__file__ not defined")
