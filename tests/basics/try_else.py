# test try-else statement

# base case
try:
    print(1)
except Exception:
    print(2)
else:
    print(3)

# basic case that should skip else
try:
    print(1)
    raise Exception
except Exception:
    print(2)
else:
    print(3)

# uncaught exception should skip else
try:
    try:
        print(1)
        raise ValueError
    except TypeError:
        print(2)
    else:
        print(3)
except Exception:
    print('caught')

# nested within outer try
try:
    print(1)
    try:
        print(2)
        raise Exception
    except Exception:
        print(3)
    else:
        print(4)
except Exception:
    print(5)
else:
    print(6)

# nested within outer except, one else should be skipped
try:
    print(1)
    raise Exception
except Exception:
    print(2)
    try:
        print(3)
    except Exception:
        print(4)
    else:
        print(5)
else:
    print(6)

# nested within outer except, both else should be skipped
try:
    print(1)
    raise Exception
except Exception:
    print(2)
    try:
        print(3)
        raise Exception
    except Exception:
        print(4)
    else:
        print(5)
else:
    print(6)
