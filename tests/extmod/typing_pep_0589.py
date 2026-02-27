try:
    import typing
except ImportError:
    print("SKIP")
    raise SystemExit

print("# Python 3.8")
print("### PEP 589")

# https://peps.python.org/topic/typing/
# https://peps.python.org/pep-0589/
# https://typing.python.org/en/latest/spec/typeddict.html#typeddict


print("Class-based Syntax")

from typing import TypedDict
from typing import NotRequired, ReadOnly, Annotated


class Movie(TypedDict):
    name: str
    year: int


class EmptyDict(TypedDict):
    pass


# ------------------------------------------------------------------------
print("Using TypedDict Types")

movie: Movie = {"name": "Blade Runner", "year": 1982}


def record_movie(movie: Movie) -> None:
    ...


record_movie({"name": "Blade Runner", "year": 1982})

movie: Movie
movie = {"name": "Blade Runner", "year": 1982}

# ------------------------------------------------------------------------
print("Totality and optional keys")

try:
    # FIXME cpy_diff: runtime typing does not accept 'total' argument
    class MovieTotal(TypedDict, total=True):
        name: str
        year: int
except TypeError as e:
    print(
        "-[ ] FIXME: cpy_diff : `total` parameter not supported by runtime TypedDict implementation:",
        e,
    )

try:

    class MoviePartial(TypedDict, total=False):
        name: str
        year: int
except TypeError as e:
    print(
        "-[ ] FIXME: cpy_diff : `total` parameter not supported by runtime TypedDict implementation:",
        e,
    )


mt: MovieTotal = {"name": "Alien", "year": 1979}  # type : ignore
mp: MoviePartial = {"name": "Alien"}  # year is optional # type : ignore

assert mt["year"] == 1979
assert "year" not in mp or isinstance(mp.get("year"), (int, type(None)))

# ------------------------------------------------------------------------
print("Inheritance and required/optional mix")


class Point2D(TypedDict):
    x: int
    y: int


try:

    class Point3D(Point2D, total=False):
        z: int
except TypeError as e:
    print(
        "FIXME: cpy_diff : `total` parameter not supported by runtime TypedDict implementation for Point3D:",
        e,
    )


p2: Point2D = {"x": 1, "y": 2}
p3: Point3D = {"x": 1, "y": 2}
assert p2["x"] == 1
assert "z" not in p3

print("Runtime checks: TypedDict cannot be used with isinstance/class checks")
try:
    if isinstance(movie, Movie):  # type: ignore
        pass
    print("-[ ] FIXME: TypedDict class allowed in isinstance (unexpected)")
except TypeError:
    print("TypedDict class not allowed for isinstance/class checks")

print("Alternative functional syntax and constructors")

MovieAlt = TypedDict("MovieAlt", {"name": str, "year": int})
MovieAlt2 = TypedDict("MovieAlt2", {"name": str, "year": int}, total=False)

m_alt: MovieAlt = {"name": "Blade Runner", "year": 1982}

# FIXME: Difference or Crash - calling the functional TypedDict constructor with kwargs
try:
    ma = MovieAlt(name="Blade Runner", year=1982)
    print(type(ma))  # should be dict at runtime
except TypeError as e:
    print(
        "-[ ] FIXME: cpy_diff Functional TypedDict constructor call failed at runtime (expected):",
        e,
    )


print("Inheritance examples")

try:

    class BookBasedMovie(Movie):
        based_on: str
except TypeError as e:
    print(
        "Inheritance from TypedDicts not supported by runtime implementation for BookBasedMovie:",
        e,
    )

# KNOWN limitation - no multiple inheritance in MicroPython
# class X(TypedDict):
#     x: int
# class Y(TypedDict):
#     y: str
# try:
#     class XYZ(X, Y):
#         z: bool
#     xyz: XYZ = {"x": 1, "y": "a", "z": True}
# except TypeError as e:
#     print("Multiple inheritance for TypedDicts not supported at runtime (XYZ):", e)

print("Totality and mixing with Required/NotRequired")


class _MovieBase(TypedDict):
    title: str


try:

    class MovieMix(_MovieBase, total=False):
        year: int
except TypeError as e:
    print(
        "FIXME: cpy_diff - total parameter not supported by runtime TypedDict implementation for MovieMix:",
        e,
    )
    MovieMix = dict  # fallback for runtime operations # type: ignore


# equivalent to marking year as NotRequired
class MovieMix2(_MovieBase):
    year: NotRequired[int]


# Do not try to execute known runtime errors:
try:
    m1: MovieMix = {}  # type: ignore
    m2: MovieMix = {"year": 2015}  # type: ignore
except TypeError as e:
    print("Assigning to MovieMix failed at runtime (expected for missing required fields):", e)

print("Required/NotRequired with Annotated/ReadOnly examples")

from typing import NotRequired, ReadOnly, Annotated


class Band(TypedDict):
    name: str
    members: ReadOnly[list[str]]


blur: Band = {"name": "blur", "members": []}
blur["name"] = "Blur"
# the following would be a type-checker error (but allowed at runtime):
blur["members"] = ["Daemon Albarn"]  # type: ignore
blur["members"].append("Daemon Albarn")

print("extra_items and closed examples")

try:

    class MovieExtra(TypedDict, extra_items=int):
        name: str

    # FIXME: Difference - constructor with kwargs
    extra_ok: MovieExtra = {"name": "BR", "year": 1982}
except TypeError as e:
    print("-[ ] FIXME: extra_items not supported by runtime typing implementation:", e)

try:

    class MovieClosed(TypedDict, closed=True):
        name: str

    try:
        # FIXME: Difference or Crash - constructor with kwargs
        MovieClosed(
            name="No Country for Old Men", year=2007
        )  # Should be runtime error per ctor semantics
        print("Constructed ClosedMovie with extra item (may be allowed at runtime)")
    except TypeError:
        print("-[ ] FIXME: Closed Movie rejected extra kwargs at construction")
except TypeError as e:
    print("-[ ] FIXME: closed parameter not supported by runtime typing implementation:", e)

print("Interaction with Mapping and dict conversions")

try:
    # FIXME:
    class IntDict(TypedDict, extra_items=int):
        pass

    not_required_num_dict: IntDict = {"num": 1, "bar": 2}
except TypeError as e:
    print("-[ ] FIXME: extra_items not supported by runtime typing implementation", e)
    # Fall back to plain dict to exercise runtime operations
    not_required_num_dict = {"num": 1, "bar": 2}
# at runtime this is a dict; operations like clear/popitem are available
not_required_num_dict.clear()

print("-----")
