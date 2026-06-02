# Testing runtime aspects of typing module
# Python 3.5+
# Miscellaneous typing module runtime tests.

try:
    from typing import TYPE_CHECKING
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest
from typing import TypeAlias


class TestTypingTypeCheckingFlag(unittest.TestCase):
    # TYPE_CHECKING-guarded import is False at runtime in MicroPython.
    def test_type_checking_branch(self):
        if TYPE_CHECKING:
            from typing_extensions import TypeGuard
        assert TYPE_CHECKING is False, "TYPE_CHECKING should be False at runtime"


class TestTypingSyntax(unittest.TestCase):
    # Module-level annotations and function with annotations should parse and run.
    MyAlias: TypeAlias = str

    def test_module_level_annotations_and_function(self):
        from typing import List, NewType, TypeVar, Union, Any

        Vector: List[float]
        UserId = NewType("UserId", int)
        T = TypeVar("T", int, float, complex)

        hintedGlobal: Any = None

        def func_with_hints(
            c: int, b: MyAlias, a: Union[int, None], lst: List[float] = [0.0]
        ) -> Any:
            pass

        func_with_hints(1, "alias", None)


class TestTypingParameterAnnotations(unittest.TestCase):
    # Parameter and return annotations are accepted and runnable.
    def test_simple_annotated_functions(self):
        from typing import Any, Dict, List

        def add_numbers(a: int, b: int) -> int:
            return a + b

        def greet(name: str) -> None:
            return None

        def get_average(numbers: List[float]) -> float:
            return sum(numbers) / len(numbers)

        def process_data(data: Dict[str, Any]) -> None:
            # process the data
            pass

        a = add_numbers(3, 5)
        greet("Alice")
        avg = get_average([1.0, 2.5, 3.5])
        process_data({"key": "value", "number": 42})
        self.assertEqual(a, 8)
        self.assertEqual(avg, sum([1.0, 2.5, 3.5]) / 3)


class TestTypingSelfPython311(unittest.TestCase):
    # typing.Self is importable and usable in annotations at runtime.
    def test_self_in_callback(self):
        from typing import Callable, Self

        class BaseClass:
            def register(self, callback: Callable[[Self], None]) -> None: ...

        def cb(x):
            pass

        base = BaseClass()
        base.register(cb)


class TestTypingNoTypeCheck(unittest.TestCase):
    # @no_type_check decorator should be a runtime no-op.
    def test_no_type_check_decorator(self):
        from typing import no_type_check

        @no_type_check
        def quad(r0):
            return r0 * 4

        self.assertEqual(quad(1), 4)


class TestTypingProtocolRuntime(unittest.TestCase):
    # Runtime use of Protocol-typed callables is duck-typed.
    def test_adder_protocol_runtime(self):
        from typing import Protocol

        class Adder(Protocol):
            def add(self, x, y): ...

        class IntAdder:
            def add(self, x, y):
                return x + y

        class FloatAdder:
            def add(self, x, y):
                return x + y

        results = []

        def add(adder: Adder) -> None:
            results.append(adder.add(2, 3))

        add(IntAdder())
        add(FloatAdder())
        self.assertEqual(results, [5, 5])


class TestTypingNewType(unittest.TestCase):
    # NewType returns an int that is an instance of int at runtime.
    def test_user_id_newtype(self):
        from typing import NewType

        UserId = NewType("UserId", int)
        some_id = UserId(524313)
        self.assertEqual(some_id, 524313)
        assert isinstance(some_id, int), "NewType should be instance of the original type"


class TestTypingAny(unittest.TestCase):
    # Any annotation accepts any value at runtime.
    def test_any_assignments(self):
        from typing import Any

        a: Any = None
        a = []  # OK
        a = 2  # OK

        s: str = ""
        s = a  # OK

    # foo() / hash_b() runtime semantics with AttributeError fallback.
    def test_hash_b_runtime(self):
        from typing import Any

        def foo(item: Any) -> int:
            # Passes type checking; 'item' could be any type,
            # and that type might have a 'bar' method
            item.bar()
            return 42

        def hash_b(item: Any) -> int:
            try:
                # Passes type checking
                item.magic()
                return foo(item)
            except AttributeError:
                # just ignore any error for this test
                pass
            return 21
            ...

        self.assertEqual(hash_b(42), 21)
        self.assertEqual(hash_b("foo"), 21)


class TestTypingAnyStr(unittest.TestCase):
    # AnyStr-typed concat works for str+str and bytes+bytes.
    def test_concat_homogeneous(self):
        from typing import AnyStr

        def concat(a: AnyStr, b: AnyStr) -> AnyStr:
            return a + b

        self.assertEqual(concat("foo", "bar"), "foobar")  # OK, output has type 'str'
        self.assertEqual(concat(b"foo", b"bar"), b"foobar")  # OK, output has type 'bytes'

    # Mixed str+bytes raises TypeError at runtime.
    def test_concat_mixed_types_raises(self):
        from typing import AnyStr

        def concat(a: AnyStr, b: AnyStr) -> AnyStr:
            return a + b

        with self.assertRaises(TypeError):
            concat("foo", b"bar")  # Error, cannot mix str and bytes # type: ignore


class TestTypingLiteralString(unittest.TestCase):
    # LiteralString annotation is runtime-permissive.
    def test_caller_uses_literal_and_runtime_str(self):
        from typing import LiteralString

        def run_query(sql: LiteralString) -> None: ...

        def caller(arbitrary_string: str, literal_string: LiteralString) -> None:
            run_query("SELECT * FROM students")  # OK
            run_query(literal_string)  # OK
            run_query("SELECT * FROM " + literal_string)  # OK
            run_query(arbitrary_string)  # type: ignore  # type checker error
            run_query(f"SELECT * FROM students WHERE name = {arbitrary_string}")  # type: ignore  # type checker error

            assert isinstance(literal_string, str), "literal_string should be a string"
            assert isinstance(arbitrary_string, str), "arbitrary_string should be a string"

        some_str = "a" * 1000
        literal_str = "drop * from tables"
        caller(some_str, literal_str)


class TestTypingOverload(unittest.TestCase):
    # @overload stacks declarations and final implementation is called at runtime.
    def test_bar_overload_runtime(self):
        from typing import overload

        @overload
        def bar(x: int) -> str: ...

        @overload
        def bar(x: str) -> int: ...

        def bar(x):
            return x

        self.assertEqual(bar(42), 42)


class TestTypingTypedDictRequired(unittest.TestCase):
    # https://typing.readthedocs.io/en/latest/spec/typeddict.html#required-and-notrequired
    # TypedDict with Required/NotRequired markers should run at runtime.
    def test_movie_required_notrequired(self):
        from typing import NotRequired, Required, TypedDict

        class Movie(TypedDict):
            title: Required[str]
            year: int
            director: NotRequired[str]

        m = Movie(title="Life of Brian", year=1979)
        self.assertEqual(m["title"], "Life of Brian")


class TestTypingTypeVar(unittest.TestCase):
    # TypeVar-based generic helper function works at runtime.
    def test_first_helper(self):
        from typing import List, TypeVar

        T = TypeVar("T")

        def first(container: List[T]) -> T:
            return container[0]

        list_one: List[str] = ["a", "b", "c"]
        list_two: List[int] = [1, 2, 3]
        self.assertEqual(first(list_one), "a")
        self.assertEqual(first(list_two), 1)


class TestTypingGenerator(unittest.TestCase):
    # Annotated Generator runs and yields values.
    def test_echo_generator(self):
        from typing import Generator

        def echo(a: float) -> Generator[int, float, str]:
            yield int(a)
            return "Done"

        e = echo(2.4)
        v = next(e)
        self.assertEqual(v, 2)


class TestTypingNoReturn(unittest.TestCase):
    # NoReturn-typed function raises as expected.
    def test_stop_no_return(self):
        from typing import NoReturn

        def stop() -> NoReturn:
            raise RuntimeError("no way")

        with self.assertRaises(RuntimeError):
            stop()


class TestTypingFinalAnnotation(unittest.TestCase):
    # Final-marked constants are normal values at runtime.
    def test_final_constant(self):
        from typing import Final

        CONST: Final = 42
        self.assertEqual(CONST, 42)


class TestTypingFinalDecorator(unittest.TestCase):
    # @final decorator and subclassing semantics at runtime.
    # FIXME cpydiff: MicroPython does not enforce @final at runtime; subclassing is silently allowed.
    @unittest.expectedFailure
    def test_final_method_and_class(self):
        from typing import final

        class Base:
            @final
            def done(self) -> None: ...

        class Sub(Base):
            def done(self) -> None:  # type: ignore # Error reported by type checker
                ...

        @final
        class Leaf: ...

        class Other(Leaf):  # type: ignore # Error reported by type checker
            ...

        other = Other()
        raise AssertionError("@final not enforced at runtime")


class TestTypingTypeVarTuple(unittest.TestCase):
    # FIXME cpydiff: TypeVarTuple/Unpack runtime support is incomplete.
    @unittest.expectedFailure
    def test_typevar_tuple_unpack(self):
        from typing import TypeVarTuple, Unpack

        Ts = TypeVarTuple("Ts")
        tup: tuple[Unpack[Ts]]  # Semantically equivalent, and backwards-compatible # type: ignore
        raise AssertionError("TypeVarTuple/Unpack not enforced at runtime")


class TestTypingParamSpec(unittest.TestCase):
    # ParamSpec, 3.11 notation
    # https://docs.python.org/3/library/typing.html#typing.ParamSpec
    # FIXME cpydiff: ParamSpec / collections.abc.Callable may not be available in MicroPython.
    @unittest.expectedFailure
    def test_add_logging_with_paramspec(self):
        from typing import Callable, ParamSpec, TypeVar

        try:
            from collections.abc import Callable as ABCCallable
        except ImportError:
            # print("- [ ] FIXME: from collections.abc import Callable")
            from typing import Callable as ABCCallable  # Workaround for test

        T = TypeVar("T")
        P = ParamSpec("P")

        def add_logging(f: ABCCallable[P, T]) -> ABCCallable[P, T]:
            """A type-safe decorator to add logging to a function."""

            def inner(*args: P.args, **kwargs: P.kwargs) -> T:
                return f(*args, **kwargs)

            return inner

        @add_logging
        def add_two(x: float, y: float) -> float:
            """Add two numbers together."""
            return x + y

        x = add_two(1, 2)
        assert x == 3, "add_two(1, 2) == 3"


class TestTypingGetOrigin(unittest.TestCase):
    # https://docs.python.org/3/library/typing.html#typing.get_origin
    # FIXME cpydiff: get_origin() unsupported, or always returns None.
    @unittest.expectedFailure
    def test_get_origin_non_none(self):
        from typing import Dict, Union, get_origin

        # if not get_origin(str) is None:
        #     print("- [ ] FIXME: document cpy_diff - get_origin(str) should be None")
        assert get_origin(Dict[str, int]) is dict
        assert get_origin(Union[int, str]) is Union


class TestTypingGetArgs(unittest.TestCase):
    # https://docs.python.org/3/library/typing.html#typing.get_args
    # FIXME cpydiff: get_args() unsupported, or always returns ().
    @unittest.expectedFailure
    def test_get_args_non_empty(self):
        from typing import Dict, Union, get_args

        # if not get_args(int) == ():
        #     print("- [ ] FIXME: document cpy_diff - get_args(int) should be ()")
        assert get_args(Dict[int, str]) == (int, str), (
            "get_args(Dict[int, str]) should be (int, str)"
        )
        assert get_args(Union[int, str]) == (int, str), (
            "get_args(Union[int, str]) should be (int, str)"
        )


class TestTypingSubscriptables(unittest.TestCase):
    # All typing subscriptable aliases declare without runtime error.
    def test_subscriptable_aliases(self):
        from typing import (
            AbstractSet,
            AsyncContextManager,
            AsyncGenerator,
            AsyncIterable,
            AsyncIterator,
            Awaitable,
        )
        from typing import (
            Callable,
            ChainMap,
            Collection,
            Container,
            ContextManager,
            Coroutine,
            Counter,
            DefaultDict,
        )
        from typing import (
            Deque,
            Dict,
            FrozenSet,
            Generator,
            Generic,
            Iterable,
            Iterator,
            List,
            Literal,
            Mapping,
        )
        from typing import (
            MutableMapping,
            MutableSequence,
            MutableSet,
            NamedTuple,
            Optional,
            OrderedDict,
            Self,
        )
        from typing import Sequence, Set, Tuple, Type, Union, Any

        t_01: AbstractSet[Any]
        t_02: AsyncContextManager[Any]
        t_03: AsyncGenerator[Any]
        t_04: AsyncIterable[Any]
        t_05: AsyncIterator[Any]
        t_06: Awaitable[Any]
        t_07: Callable[[], Any]
        t_08: ChainMap[Any, Any]
        t_09: Collection[Any]
        t_10: Container[Any]
        t_11: ContextManager[Any]
        t_12: Coroutine[Any, Any, Any]
        t_13: Counter[Any]
        t_14: DefaultDict[Any, Any]
        t_15: Deque[Any]
        t_16: Dict[Any, Any]
        t_17: FrozenSet[Any]
        t_18: Generator[Any]
        # t_19: Generic[Any] # Not supported in MicroPython
        t_20: Iterable[Any]
        t_21: Iterator[Any]
        t_22: List[Any]
        t_23: Literal[1, 2, 3, "a", b"b", True, None]
        t_24: Mapping[Any, Any]
        t_25: MutableMapping[Any, Any]
        t_26: MutableSequence[Any]
        t_27: MutableSet[Any]
        t_28: NamedTuple
        t_29: Optional[Any]
        t_30: OrderedDict[Any, Any]
        # t_31: Self[Any] # Not supported in MicroPython
        t_32: Sequence[Any]
        t_33: Set[Any]
        t_34: Tuple[Any]
        t_35: Type[Any]
        t_36: Union[Any, Any]


if __name__ == "__main__":
    unittest.main()
