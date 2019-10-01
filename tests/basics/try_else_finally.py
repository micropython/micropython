# test try-else-finally statement

# base case
try:
    print(1)
except:
    print(2)
else:
    print(3)
finally:
    print(4)

# basic case that should skip else
try:
    print(1)
    raise Exception
except:
    print(2)
else:
    print(3)
finally:
    print(4)

# uncaught exception should skip else
try:
    try:
        print(1)
        raise ValueError
    except TypeError:
        print(2)
    else:
        print(3)
    finally:
        print(4)
except:
    print('caught')

# nested within outer try
try:
    print(1)
    try:
        print(2)
        raise Exception
    except:
        print(3)
    else:
        print(4)
    finally:
        print(5)
except:
    print(6)
else:
    print(7)
finally:
    print(8)

# nested within outer except, one else should be skipped
try:
    print(1)
    raise Exception
except:
    print(2)
    try:
        print(3)
    except:
        print(4)
    else:
        print(5)
    finally:
        print(6)
else:
    print(7)
finally:
    print(8)

# nested within outer except, both else should be skipped
try:
    print(1)
    raise Exception
except:
    print(2)
    try:
        print(3)
        raise Exception
    except:
        print(4)
    else:
        print(5)
    finally:
        print(6)
else:
    print(7)
finally:
    print(8)
