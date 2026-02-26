# test errors from bad function calls
try:
    enumerate
except Exception:
    print("SKIP")
    raise SystemExit

# function with keyword args not given a specific keyword arg
try:
    enumerate()
except TypeError:
    print('TypeError')
