try:
    import traceback
except ImportError:
    print("SKIP")
    raise SystemExit


def fun():
    raise Exception("test")


try:
    fun()
except Exception as exc:
    print("\nNo Trace:")
    traceback.print_exception(None, exc, None)
    print("\nDefault Trace:")
    traceback.print_exception(None, exc, exc.__traceback__)
    print("\nLimit=1 Trace:")
    traceback.print_exception(None, exc, exc.__traceback__, limit=1)
    print("\nLimit=0 Trace:")
    traceback.print_exception(None, exc, exc.__traceback__, limit=0)
    print("\nLimit=-1 Trace:")
    traceback.print_exception(None, exc, exc.__traceback__, limit=-1)


class NonNativeException(Exception):
    pass


try:
    raise NonNativeException("test")
except Exception as e:
    print("\nNonNative Trace:")
    traceback.print_exception(None, e, e.__traceback__)
    print("")
