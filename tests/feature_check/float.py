# detect how many bits of precision the floating point implementation has

try:
    float
except NameError:
    print(0)
else:
    if float("1.0000001") == float("1.0"):
        print(30)
    elif float("1e300") == float("inf"):
        print(32)
    else:
        print(64)
