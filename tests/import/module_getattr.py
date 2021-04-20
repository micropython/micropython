# test __getattr__ on module

# ensure that does_not_exist doesn't exist to start with
this = __import__(__name__)
try:
    this.does_not_exist
    assert False
except AttributeError:
    pass

# define __getattr__
def __getattr__(attr):
    if attr == "does_not_exist":
        return False
    raise AttributeError


# do feature test (will also test functionality if the feature exists)
if not hasattr(this, "does_not_exist"):
    print("SKIP")
    raise SystemExit

# check that __getattr__ works as expected
print(this.does_not_exist)
