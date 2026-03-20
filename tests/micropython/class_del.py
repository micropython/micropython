"""Test user-class __del__ finalisers.

Note: Like CPython, MicroPython's GC is conservative --- there is no guarantee
it will collect every object that a theoretical abstract Python Virtual Machine
could. In practice, it's still useful, but objects can still remain latent in
"stack holes", i.e. uninitialized struct fields that the compiler has aliased
with other stack variables that have mutually-exclusive lifetimes.

The only comprehensive solution to this that this author is aware of is GCC's
`-ftrivial-auto-var-init=zero`, at a cost of +10k code size and a serious
runtime performance hit. Therefore, this perfect consistency is not attempted:
this test only tests for eventual consistency, i.e. at steady state, objects
that are not interacted with all eventually get collected --- but it's accepted
that a small O(1) handful of objects might remain permanently latent, plus those
created very early or touched very recently.
"""

try:
    import unittest
    import micropython
    import gc
    import sys

    gc.is_finalized
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


if sys.platform == "webassembly" and sys.implementation._build == "pyscript":
    # GC cannot run in the middle of python bytecode on this platform.
    print("SKIP")
    raise SystemExit


class Test(unittest.TestCase):
    NUM_TO_PRIME = 100
    """Create this many objects at the start, to prime the system to steady-state."""
    NUM_TO_CHECK = 100
    """Create this many objects to test the system and verify finalisers are run."""
    NUM_TO_FLUSH = 100
    """Create this many objects at the end, to flush any latent finalisers."""
    ALLOWED_MISS = 2
    """Up to this many of the checked objects are allowed to be missed."""

    RANGE = list(range(NUM_TO_PRIME + NUM_TO_CHECK + NUM_TO_FLUSH))

    @staticmethod
    def collect_tryveryhard():
        gc.collect()
        try:
            raise Exception  # nlr.ret_val seems to be a major culprit for spurious reachability
        except Exception:
            pass
        gc.collect()

    def setUp(self):
        self.finalised: dict[int, bool] = dict.fromkeys(self.RANGE, False)
        self.collect_tryveryhard()

    def check_final(self):
        self.collect_tryveryhard()
        missed = []

        for idx, ran in self.finalised.items():
            base = 0

            if idx < base + self.NUM_TO_PRIME:
                continue
            base += self.NUM_TO_PRIME

            if idx < base + self.NUM_TO_CHECK:
                if not ran:
                    missed.append(idx)
                continue
            base += self.NUM_TO_CHECK

            if idx < base + self.NUM_TO_FLUSH:
                continue
            base += self.NUM_TO_FLUSH

            raise Exception("Unknown index {}!".format(idx))

        if len(missed) > self.ALLOWED_MISS:
            self.assertEqual(missed, [])

    def test_del_simple(self):
        """Create simple independent objects."""

        class Simple:
            def __init__(self2, i):
                self2.i = i

            def __del__(self2):
                self.finalised[self2.i] = True

        for i in self.RANGE:
            obj = Simple(i)

        del obj
        self.check_final()

    def test_del_preinsert(self):
        """Insert a finaliser after class creation, before instantiation."""

        class PreInsert:
            def __init__(self2, i):
                self2.i = i

        def finaliser(self2):
            self.finalised[self2.i] = True

        PreInsert.__del__ = finaliser

        for i in self.RANGE:
            obj = PreInsert(i)

        del obj
        self.check_final()

    def test_del_postinsert(self):
        """Attempt to insert a finaliser after instantiation."""

        class PostInsert:
            def __init__(self2, i):
                self2.i = i

        def finaliser(self2):
            self.finalised[self2.i] = True

        retained = []
        for i in self.RANGE:
            obj = PostInsert(i)
            retained.append(obj)

        try:
            PostInsert.__del__ = finaliser
        except AttributeError:
            self.skipTest("Correctly errored on feature non-presence.")

        while retained:
            obj = retained.pop(0)
        del obj
        self.check_final()

    def test_del_postreplace(self):
        """Replace an existing finaliser with a different one after instantiation."""

        class PostReplace:
            def __init__(self2, i):
                self2.i = i

            def __del__(self2):
                pass  # don't report anything

        def finaliser(self2):
            self.finalised[self2.i] = True

        retained = []
        for i in self.RANGE:
            obj = PostReplace(i)
            retained.append(obj)

        PostReplace.__del__ = finaliser

        while retained:
            obj = retained.pop(0)
        del obj
        self.check_final()

    def test_del_child(self):
        """Add a finaliser after instantiation by inserting the finalisable object into its object graph."""

        class Parent:
            def __init__(self2, i):
                self2.i = i
                self2.parent = None

        class Child:
            def __init__(self2, parent):
                self2.parent = parent  # couple their lifetimes
                self2.parent.__child = self2

            def __del__(self2):
                self.finalised[self2.parent.i] = True

        for i in self.RANGE:
            obj = Parent(i)
            Child(obj)

        del obj
        self.check_final()

    def test_del_necromancy(self):
        """Test what happens when a finaliser tries to make its object reachable again."""

        zombie_range = [i + 1000 for i in self.RANGE]
        sentinel = object()
        graveyard = dict.fromkeys(self.RANGE + zombie_range, sentinel)

        class Zombie:
            def __init__(self2, i):
                self2.i = i

            def __del__(self2):
                graveyard[self2.i] = self2

        for i in self.RANGE:
            obj = Zombie(i)

        del obj

        self.collect_tryveryhard()

        retained = []
        for i in zombie_range:
            obj = Zombie(i)
            retained.append(obj)

        del obj

        self.collect_tryveryhard()

        for obj in retained:
            self.assertFalse(gc.is_finalized(obj))

        for i, obj in graveyard.items():
            if obj == sentinel:
                pass
            else:
                self.assertEqual(obj.i, i)
                self.assertTrue(gc.is_finalized(obj))


if __name__ == "__main__":
    unittest.main()
