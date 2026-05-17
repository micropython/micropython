class CtxMgr:

    def __enter__(self):
        print("__enter__")
        return self

    def __exit__(self, a, b, c):
        print("__exit__", repr(a), repr(b))


with CtxMgr() as a:
    print(isinstance(a, CtxMgr))

try:
    with CtxMgr() as a:
        raise ValueError
except ValueError:
    print("ValueError")


class CtxMgr2:

    def __enter__(self):
        print("__enter__")
        return self

    def __exit__(self, a, b, c):
        print("__exit__", repr(a), repr(b))
        return True

try:
    with CtxMgr2() as a:
        raise ValueError
    print("No ValueError2")
except ValueError:
    print("ValueError2")


# These recursive try-finally tests are attempt to get some interpretation
# of last phrase in http://docs.python.org/3.4/library/dis.html#opcode-WITH_CLEANUP
# "If the stack represents an exception, and the function call returns a 'true'
# value, this information is "zapped" and replaced with a single WHY_SILENCED
# to prevent END_FINALLY from re-raising the exception. (But non-local gotos
# will still be resumed.)"
print("===")
with CtxMgr2() as a:
    try:
        try:
            raise ValueError
            print("No ValueError3")
        finally:
            print("finally1")
    finally:
        print("finally2")

print("===")
try:
    try:
        with CtxMgr2() as a:
            try:
                try:
                    raise ValueError
                    print("No ValueError3")
                finally:
                    print("finally1")
            finally:
                print("finally2")
    finally:
        print("finally3")
finally:
    print("finally4")
