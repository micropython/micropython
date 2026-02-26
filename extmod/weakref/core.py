import _weakref
import micropython
import sys

try:
    from sys import atexit as _link_atexit
except ImportError:
    try:
        from atexit import register as _register_atexit

        def _link_atexit(f):
            _register_atexit(f)
            return None
    except ImportError:

        def _link_atexit(f):
            return None


# Any.__weakref__: list[_target] | del
class _target(_weakref.ref):
    def __init__(self, obj, **riders):
        super().__init__(obj)

        for k, v in riders.items():
            setattr(self, k, v)

    def _inject(self, parent):
        try:
            parent.__weakref__.append(self)
        except AttributeError:
            try:
                setattr(parent, "__weakref__", [self])
            except AttributeError:
                raise TypeError(
                    "cannot create weak reference to '{}' object".format(type(parent).__name__)
                )


# https://docs.python.org/3/library/weakref.html#weakref.getweakrefcount
def getweakrefcount(obj) -> int:
    try:
        return len(obj.__weakref__)
    except AttributeError:
        return 0


# https://docs.python.org/3/library/weakref.html#weakref.getweakrefs
def getweakrefs(obj) -> list:
    def genweakrefs():
        for tgt in getattr(obj, "__weakref__", []):
            ptr = tgt()
            if ptr is not None:
                yield tgt

    return list(genweakrefs())


# https://docs.python.org/3/library/weakref.html#weakref.ref
class ref(_weakref.ref):
    def __call__(self):
        tgt = super().__call__()
        return getattr(tgt, "parent", None)

    def __init__(self, obj, callback=None, **riders):
        # self._sched_run = None

        tgt = _target(self, parent=obj, **riders)
        super().__init__(tgt)
        tgt._inject(obj)

        self._sched_next = None
        self._sched_run = callback

    # _sched_head is a linked list on the `_sched_next` attribute
    # so that we can schedule a whole chain of callbacks, without allocating,
    # using only a single micropython.schedule slot
    _sched_head = None

    def __callback__(self, arg):
        super().__callback__(arg)

        if self._sched_next is None and self._sched_run is not None:
            self._sched_next = ref._sched_head
            ref._sched_head = self

            if self._sched_next is None:  # i.e. if there was not already a schedule_head
                micropython.schedule(ref._sched_loop, None)

    @staticmethod
    def _sched_loop(arg):
        while ref._sched_head is not None:
            entry = ref._sched_head
            ref._sched_head = entry._sched_next
            entry._sched_next = None  # unlink list to minimize spurious reachability effects

            if entry._sched_run is None:
                continue

            try:
                entry._sched_run(entry)
            except Exception as e:
                print("Unhandled exception in weakref callback:")
                sys.print_exception(e)


# https://docs.python.org/3/library/weakref.html#weakref.finalize
class finalize:
    def __init__(self, obj, func, *args, **kwargs):
        self._alive = False
        self._func = func
        self._args = args
        self._kwargs = kwargs

        self._ptr = ref(obj, callback=self, owner=self)

        self._atexit_next = _link_atexit(self._atexit)
        self.atexit = True

        self._alive = True

    @property
    def alive(self):
        obj = self._ptr()
        return self._alive and obj is not None

    def peek(self):
        obj = self._ptr()
        if self._alive and obj is not None:
            return self._ptr(), self._func, self._args, self._kwargs

    def detach(self):
        obj = self._ptr()
        if self._alive and obj is not None:
            self._alive = False
            return self._ptr(), self._func, self._args, self._kwargs

    def __call__(self, arg=None):
        if self._alive:
            self._alive = False
            return self._func(*self._args, **self._kwargs)

    def __del__(self):
        if self._alive:
            self._alive = False
            self._func(*self._args, **self._kwargs)  # must strip return value

    def _atexit(self):
        try:
            if self.atexit:
                self()
        finally:
            if self._atexit_next is not None:
                self._atexit_next()
