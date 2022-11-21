# test subclassing custom native class

try:
    from cexample import AdvancedTimer
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
