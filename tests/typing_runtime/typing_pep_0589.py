# Python 3.8
# PEP 589 - TypedDict
# https://peps.python.org/topic/typing/
# https://peps.python.org/pep-0589/
# https://typing.python.org/en/latest/spec/typeddict.html#typeddict

try:
    from typing import TYPE_CHECKING
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

from typing import TypedDict
from typing import NotRequired


class TestPep589ClassBasedSyntax(unittest.TestCase):
    # Class-based TypedDict definitions parse and instantiate at runtime.
    def test_class_based_typeddict_definition(self):
        class Movie(TypedDict):
            name: str
            year: int

        class EmptyDict(TypedDict):
            pass

        movie: Movie = {"name": "Blade Runner", "year": 1982}
        self.assertEqual(movie["name"], "Blade Runner")


class TestPep589UsingTypedDictTypes(unittest.TestCase):
    # Function accepting a TypedDict accepts a plain dict at runtime.
    def test_typed_dict_function_argument(self):
        class Movie(TypedDict):
            name: str
            year: int

        def record_movie(movie: Movie) -> None: ...

        record_movie({"name": "Blade Runner", "year": 1982})

        movie: Movie
        movie = {"name": "Blade Runner", "year": 1982}
        self.assertEqual(movie["year"], 1982)


class TestPep589TotalityAndOptionalKeys(unittest.TestCase):
    # FIXME cpy_diff: runtime typing does not accept 'total' argument on TypedDict.
    @unittest.expectedFailure
    def test_typeddict_total_true(self):
        class MovieTotal(TypedDict, total=True):
            name: str
            year: int

        mt: MovieTotal = {"name": "Alien", "year": 1979}  # type : ignore
        self.assertEqual(mt["year"], 1979)

    @unittest.expectedFailure
    def test_typeddict_total_false(self):
        class MoviePartial(TypedDict, total=False):
            name: str
            year: int

        mp: MoviePartial = {"name": "Alien"}  # year is optional # type : ignore
        self.assertTrue("year" not in mp or isinstance(mp.get("year"), (int, type(None))))


class TestPep589InheritanceMix(unittest.TestCase):
    # Single inheritance from TypedDict with new fields should work at runtime.
    def test_point2d_typed_dict(self):
        class Point2D(TypedDict):
            x: int
            y: int

        p2: Point2D = {"x": 1, "y": 2}
        self.assertEqual(p2["x"], 1)

    # FIXME cpy_diff: `total` parameter on subclass not supported.
    @unittest.expectedFailure
    def test_point3d_total_false_subclass(self):
        class Point2D(TypedDict):
            x: int
            y: int

        class Point3D(Point2D, total=False):
            z: int

        p3: Point3D = {"x": 1, "y": 2}
        self.assertTrue("z" not in p3)


class TestPep589RuntimeIsInstance(unittest.TestCase):
    # TODO: TypedDict class should not be allowed in isinstance checks.
    @unittest.expectedFailure
    def test_typeddict_isinstance_check(self):
        class Movie(TypedDict):
            name: str
            year: int

        movie: Movie = {"name": "Blade Runner", "year": 1982}
        # try:
        if isinstance(movie, Movie):  # type: ignore
            pass
        # print("-[ ] FIXME: TypedDict class allowed in isinstance (unexpected)")
        # except TypeError:
        #     print("TypedDict class not allowed for isinstance/class checks")
        raise TypeError("TypedDict not allowed in isinstance")


class TestPep589FunctionalSyntax(unittest.TestCase):
    # Functional syntax MovieAlt = TypedDict("MovieAlt", {...}) is not supported.
    @unittest.expectedFailure
    def test_functional_typeddict_construction(self):
        # cpydiff: cannot construct a TypedDict with the functional syntax in MicroPython
        MovieAlt = TypedDict("MovieAlt", {"name": str, "year": int})

        m_alt: MovieAlt = {"name": "Blade Runner", "year": 1982}
        self.assertEqual(m_alt["name"], "Blade Runner")

    @unittest.expectedFailure
    def test_functional_typeddict_constructor_kwargs(self):
        # cpydiff: cannot construct a TypedDict with keyword arguments in MicroPython
        MovieAlt2 = TypedDict("MovieAlt2", {"name": str, "year": int}, total=False)
        ma = MovieAlt2(name="Blade Runner", year=1982)
        self.assertTrue(isinstance(ma, dict))


class TestPep589InheritanceExamples(unittest.TestCase):
    # Subclassing a TypedDict with additional fields should not raise at runtime.
    def test_book_based_movie_inherits_movie(self):
        class Movie(TypedDict):
            name: str
            year: int

        try:

            class BookBasedMovie(Movie):
                based_on: str
        except TypeError as e:
            self.fail("Inheritance from TypedDicts failed at runtime: {}".format(e))

    # KNOWN limitation - no multiple inheritance in MicroPython.
    @unittest.expectedFailure
    def test_multiple_inheritance_typed_dicts(self):
        class X(TypedDict):
            x: int

        class Y(TypedDict):
            y: str

        class XYZ(X, Y):
            z: bool

        xyz: XYZ = {"x": 1, "y": "a", "z": True}
        self.assertEqual(xyz["x"], 1)


class TestPep589TotalityWithRequiredNotRequired(unittest.TestCase):
    # TODO cpy_diff - total parameter not supported by runtime TypedDict for inherited classes.
    @unittest.expectedFailure
    def test_movie_mix_with_total_false(self):
        class _MovieBase(TypedDict):
            title: str

        class MovieMix(_MovieBase, total=False):
            year: int

        m1: MovieMix = {}  # type: ignore
        m2: MovieMix = {"year": 2015}  # type: ignore
        self.assertEqual(m2["year"], 2015)

    # NotRequired field equivalent of total=False should run at runtime.
    def test_movie_mix2_with_notrequired(self):
        class _MovieBase(TypedDict):
            title: str

        # equivalent to marking year as NotRequired
        class MovieMix2(_MovieBase):
            year: NotRequired[int]

        instance: MovieMix2 = {"title": "X"}  # type: ignore
        self.assertEqual(instance["title"], "X")


class TestPep589AnnotatedReadOnly(unittest.TestCase):
    # ReadOnly[List[str]] is a typing-only marker; runtime mutation still works.
    def test_band_with_readonly_members(self):
        from typing import ReadOnly

        class Band(TypedDict):
            name: str
            members: ReadOnly[list[str]]

        blur: Band = {"name": "blur", "members": []}
        blur["name"] = "Blur"
        # the following would be a type-checker error (but allowed at runtime):
        blur["members"] = ["Daemon Albarn"]  # type: ignore
        blur["members"].append("Daemon Albarn")
        self.assertEqual(len(blur["members"]), 2)


class TestPep589ExtraItemsAndClosed(unittest.TestCase):
    # FIXME: extra_items not supported by runtime typing implementation.
    @unittest.expectedFailure
    def test_typeddict_extra_items_int(self):
        class MovieExtra(TypedDict, extra_items=int):
            name: str

        # FIXME: Difference - constructor with kwargs
        extra_ok: MovieExtra = {"name": "BR", "year": 1982}
        self.assertEqual(extra_ok["name"], "BR")

    # FIXME: closed parameter not supported by MicroPython.
    # TypeError: function doesn't take keyword arguments on classdefinition.
    @unittest.expectedFailure
    def test_typeddict_closed_true(self):
        class MovieClosed(TypedDict, closed=True):
            name: str

        # FIXME: Difference or Crash - constructor with kwargs
        MovieClosed(name="No Country for Old Men", year=2007)
        # Should be runtime error per ctor semantics


class TestPep589MappingInteraction(unittest.TestCase):
    # FIXME: extra_items not supported by runtime typing implementation.
    # TypeError: function doesn't take keyword arguments on classdefinition.
    @unittest.expectedFailure
    def test_intdict_extra_items_and_clear(self):
        class IntDict(TypedDict, extra_items=int):
            pass

        not_required_num_dict: IntDict = {"num": 1, "bar": 2}
        # at runtime this is a dict; operations like clear/popitem are available
        not_required_num_dict.clear()
        self.assertEqual(not_required_num_dict, {})


if __name__ == "__main__":
    unittest.main()
