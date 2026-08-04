# test subclassing custom native class

try:
    from cexample import AdvancedTimer
    import time  # used to skip this test on minimal unix variant
except ImportError:
    print("SKIP")
    raise SystemExit


class SubTimer(AdvancedTimer):
    def __init__(self):
        # At this point, self does not yet represent a AdvancedTimer instance.
        print(self)

        # So lookups via type.attr handler will fail.
        try:
            self.seconds
        except AttributeError:
            print("AttributeError")

        # Also applies to builtin methods.
        try:
            self.time()
        except AttributeError:
            print("AttributeError")

        # Initialize base class.
        super().__init__(self)

        # Now you can access methods and attributes normally.
        self.time()
        print(self.seconds)
        self.seconds = 123
        print(self.seconds)


watch = SubTimer()

# Storing the native "seconds" property must be delegated to the native attr
# handler and not shadowed in the instance dict.
# See https://github.com/micropython/micropython/issues/18592.
print("seconds" in watch.__dict__)

# Attributes that the native handler does not own fall through to normal
# instance-dict storage, so a subclass can still hold arbitrary attributes.
watch.extra = "hello"
print(watch.extra)
print("extra" in watch.__dict__)
