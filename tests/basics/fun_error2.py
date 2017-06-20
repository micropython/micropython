# test errors from bad function calls
try:
    enumerate
except:
    print("SKIP")
    raise SystemExit

def test_exc(code, exc):
    try:
        exec(code)
        print("no exception")
    except exc:
        print("right exception")
    except:
        print("wrong exception")

# function with keyword args not given a specific keyword arg
test_exc("enumerate()", TypeError)
