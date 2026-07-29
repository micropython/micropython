# Python 3.6
# PEP 526 - Syntax for variable annotations
# https://peps.python.org/pep-0526/
#
# Currently excludes tests using `Generic[T]` due to MicroPython runtime limitations

try:
    from typing import TYPE_CHECKING
except Exception:
    print("SKIP")
    raise SystemExit

import unittest

from typing import List, Tuple, Optional, ClassVar, Dict


def xfail_on_error(func):
    """Report test as skipped ("xfail: ...") if it raises, or as ok if it
    passes. Use instead of @unittest.expectedFailure when an unexpected pass
    should NOT be reported as a failure (xpass)."""

    def wrapper(self):
        try:
            func(self)
        except Exception as e:
            raise unittest.SkipTest("xfail: {}".format(e))

    return wrapper


class TestPep526Specification(unittest.TestCase):
    # Basic variable annotations at function/module scope.
    def test_basic_variable_annotation(self):
        my_var: int
        my_var = 5  # Passes type check.
        other_var: int = "a"  # Flagged as error by type checker, # type: ignore
        # but OK at runtime.

        self.assertEqual(my_var, 5)
        self.assertEqual(other_var, "a")


class TestPep526GlobalLocalAnnotations(unittest.TestCase):
    # Variable annotation without initial value at module scope.
    def test_uninitialized_annotation(self):
        some_number: int  # variable without initial value
        some_list: List[int] = []  # variable with initial value

        self.assertEqual(some_list, [])

    # Conditional assignment with annotation.
    def test_conditional_annotated_assignment(self):
        sane_world: bool
        if 2 + 2 == 4:
            sane_world = True
        else:
            sane_world = False

        self.assertTrue(sane_world)

    # Tuple packing with variable annotation syntax.
    def test_tuple_annotation_packing(self):
        t: Tuple[int, ...] = (1, 2, 3)
        self.assertEqual(t, (1, 2, 3))
        # This only works in Python 3.8+
        t2: Tuple[int, ...] = 1, 2, 3
        self.assertEqual(t2, (1, 2, 3))

    # Tuple unpacking style annotation declarations.
    def test_tuple_unpacking_annotations(self):
        header: str
        kind: int
        body: Optional[List[str]]
        # Just verifies that the annotation statements parse and execute at runtime.

    # An annotated module-level name is not bound by the annotation alone.
    def test_module_scope_annotation_no_binding(self):
        a: int  # type: ignore
        with self.assertRaises(NameError):
            print(a)  # raises NameError # type: ignore

    # Annotated local names that are not assigned raise UnboundLocalError on use.
    @unittest.expectedFailure
    def test_local_scope_annotation_unbound(self):
        def f_1():
            a: int
            try:
                print(a)  # raises UnboundLocalError # type: ignore
            # FIXME: CPYDIFF MicroPython raises NameError instead of UnboundLocalError
            except UnboundLocalError:
                return "cpython_compliant"
            except NameError:
                return "fail"
            return "fail"

        self.assertEqual(f_1(), "cpython_compliant")

    # Re-annotating the same name with a different type is permitted at runtime.
    def test_reannotation_runtime(self):
        a: int  # type: ignore
        a: str  # Static type checker may or may not warn about this.
        a = "hello"
        self.assertEqual(a, "hello")


class TestPep526ClassAndInstanceAnnotations(unittest.TestCase):
    # Basic class with ClassVar and instance annotations.
    def test_basic_starship_class(self):
        class BasicStarship:
            captain: str = "Picard"  # instance variable with default
            damage: int  # instance variable without default
            stats: ClassVar[Dict[str, int]] = {}  # class variable

        self.assertEqual(BasicStarship.captain, "Picard")
        self.assertEqual(BasicStarship.stats, {})

    # Starship with __init__ that sets instance fields.
    def test_starship_with_init_and_hit(self):
        class Starship:
            captain: str = "Picard"
            damage: int
            stats: ClassVar[Dict[str, int]] = {}

            def __init__(self, damage: int, captain: str = None):  # type: ignore
                self.damage = damage
                if captain:
                    self.captain = captain  # Else keep the default

            def hit(self):
                Starship.stats["hits"] = Starship.stats.get("hits", 0) + 1

        enterprise_d = Starship(3000)
        enterprise_d.stats = {}  # Flagged as error by a type checker # type: ignore
        Starship.stats = {}  # This is OK
        enterprise_d.hit()
        self.assertEqual(Starship.stats.get("hits"), 1)


class TestPep526Generics(unittest.TestCase):
    @xfail_on_error
    def test_user_defined_generic_class(self):
        from typing import Generic, TypeVar

        T = TypeVar("T")

        class Box(Generic[T]):
            def __init__(self, content):
                self.content: T = content

        Box(1)


class TestPep526AnnotatingExpressions(unittest.TestCase):
    # Annotating attributes and subscript targets.
    def test_attribute_and_subscript_annotations(self):
        class Cls:
            pass

        c = Cls()
        c.x: int = 0  # Annotates c.x with int. # type: ignore
        c.y: int  # Annotates c.y with int.# type: ignore

        d = {}
        d["a"]: int = 0  # Annotates d['a'] with int.# type: ignore
        d["b"]: int  # Annotates d['b'] with int.# type: ignore

        self.assertEqual(c.x, 0)
        self.assertEqual(d["a"], 0)

    # Annotated parenthesized names are valid expressions.
    def test_parenthesized_name_annotation(self):
        (x): int  # Annotates x with int, (x) treated as expression by compiler.# type: ignore
        (y): int = 0  # Same situation here.
        self.assertEqual(y, 0)

    # print("Where annotations aren’t allowed")
    # The Examples crash both CPython and MicroPython at runtime.


class TestPep526RuntimeEffects(unittest.TestCase):
    # An undefined type name used in an annotation is a runtime no-op when the
    # annotation appears in a function body and is not actually evaluated.
    def test_annotation_with_undefined_name_in_function_body(self):
        def f():
            x: NonexistentName  # No RUNTIME error. # type: ignore

        f()

    # FIXME: cpy_diff - MicroPython does not evaluate annotations, so no NameError is raised.
    @unittest.expectedFailure
    def test_module_level_annotation_undefined_name_raises(self):
        with self.assertRaises(NameError):
            x: NonexistentName  # Error!  # type: ignore

    # FIXME: cpy_diff - MicroPython does not evaluate annotations, so no NameError is raised.
    @unittest.expectedFailure
    def test_class_body_annotation_undefined_name_raises(self):
        with self.assertRaises(NameError):

            class X:
                var: NonexistentName  # Error!  # type: ignore

    # FIXME: cpy_diff - MicroPython does not provide ``__annotations__`` at runtime.
    @unittest.expectedFailure
    def test_module_annotations_dict_runtime(self):
        # print(__annotations__)
        __annotations__["s"] = str  # noqa: F821

    # String annotations with arbitrary content are accepted at runtime.
    def test_string_annotations_runtime(self):
        alice: "well done" = "A+"  # type: ignore
        bob: "what a shame" = "F-"  # type: ignore
        self.assertEqual(alice, "A+")
        self.assertEqual(bob, "F-")


if __name__ == "__main__":
    unittest.main()
