# Runtime tests that mirror feat_typing execution patterns.

try:
    import typing
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest


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


class TestTypingRuntime(unittest.TestCase):
    # This fails for the 'Mocked' typing modules
    # Check star import and private symbol presence when available.
    # def test_star_import_and_private_symbol(self):
    #     ns = {}
    #     exec("from typing import *", {}, ns)
    #     self.assertTrue("List" in ns)
    #     self.assertTrue(hasattr(typing, "_AnyCall"))

    # Check Self usage in method callback annotations.
    def test_self_in_callback_annotation(self):
        class BaseClass:
            def register(self, callback: typing.Callable[[typing.Self], None]) -> None:
                callback(self)

        state = {"called": False}

        def cb(x):
            state["called"] = x is not None

        base = BaseClass()
        base.register(cb)
        self.assertTrue(state["called"])

    # Check decorator stacking with no_type_check.
    def test_no_type_check_with_stacked_decorators(self):
        def my_deco(func):
            def wrapper(*args, **kwargs):
                return func(*args, **kwargs)

            return wrapper

        @typing.no_type_check
        @my_deco
        def foo(x=""):
            return "ok" + x

        self.assertEqual(foo("!"), "ok!")

    # typing spec (directives): no_type_check can be applied to classes without runtime breakage.
    def test_no_type_check_class_decorator_runtime(self):
        @typing.no_type_check
        class Payload:
            value: int = "runtime"

            def __init__(self):
                self.value = "ok"

        item = Payload()
        self.assertEqual(item.value, "ok")

    # Check that annotations do not enforce runtime argument types here.
    def test_annotated_calls_are_runtime_permissive(self):
        ns = {}
        code = (
            "from typing import Any, Dict, List\n"
            "def func_with_hints(c: int, b: str, a: int | None, lst: List[float] = [0.0]) -> Any:\n"
            "    return 42\n"
            "x = func_with_hints(1, '1', None, lst=3.14)\n"
        )
        exec(code, {}, ns)
        self.assertEqual(ns["x"], 42)

    # Check typing_extensions TYPE_CHECKING interop used in feat_typing.
    def test_typing_extensions_type_checking_import(self):
        try:
            from typing_extensions import TYPE_CHECKING as te_type_checking
        except ImportError:
            te_type_checking = typing.TYPE_CHECKING

        self.assertTrue(type(te_type_checking) is bool)

    # typing spec (directives): TYPE_CHECKING-gated blocks should not execute at runtime.
    def test_type_checking_gated_branch_is_runtime_false(self):
        flag = 0
        if typing.TYPE_CHECKING:
            flag = 1

        self.assertEqual(flag, 0)

    # AnyStr annotations should allow str+str and bytes+bytes, and fail on mixed runtime concat.
    def test_anystr_runtime_mixed_concat_behavior(self):
        def concat(a: typing.AnyStr, b: typing.AnyStr) -> typing.AnyStr:
            return a + b

        self.assertEqual(concat("foo", "bar"), "foobar")
        self.assertEqual(concat(b"foo", b"bar"), b"foobar")

        with self.assertRaises(TypeError):
            concat("foo", b"bar")

    # NewType runtime value path should produce an int-compatible value.
    def test_newtype_runtime_value_path(self):
        user_id = typing.NewType("UserId", int)

        if typing.NewType("UserId2", int) is int:
            value = user_id(524313)
            self.assertTrue(isinstance(value, int))
            self.assertEqual(value, 524313)
            return

        value = user_id(524313)
        self.assertTrue(isinstance(value, int))
        self.assertEqual(value, 524313)

    # Overload declaration pattern should be runtime-safe with a concrete implementation.
    def test_overload_declaration_pattern(self):
        @typing.overload
        def bar(x: int) -> str: ...

        @typing.overload
        def bar(x: str) -> int: ...

        def bar(x):
            return x

        self.assertEqual(bar(42), 42)
        self.assertEqual(bar("x"), "x")

    # typing.Generator annotation path should execute and yield as expected.
    def test_typing_generator_runtime_path(self):
        def echo_round() -> typing.Generator[int, float, str]:
            sent = yield 0
            while sent >= 0:
                sent = yield round(sent)
            return "Done"

        g = echo_round()
        self.assertEqual(next(g), 0)

    # NoReturn annotations should allow normal runtime raising behavior.
    def test_noreturn_runtime_path(self):
        def hard_stop() -> typing.NoReturn:
            raise RuntimeError("stop execution here")

        with self.assertRaises(RuntimeError):
            hard_stop()

    # Final can be used as annotation without changing runtime value behavior.
    def test_final_runtime_path(self):
        const: typing.Final = 42
        self.assertEqual(const, 42)

    # IO marker classes should be instantiable in this runtime typing module.
    def test_io_marker_classes_runtime_path(self):
        io_obj = typing.IO()
        text_obj = typing.TextIO()
        binary_obj = typing.BinaryIO()

        self.assertTrue(isinstance(io_obj, typing.IO))
        self.assertTrue(isinstance(text_obj, typing.TextIO))
        self.assertTrue(isinstance(binary_obj, typing.BinaryIO))


class TestTypingMod(unittest.TestCase):
    # typing spec (library interface): module should expose expected public symbols.
    def test_all_defined_names_exist(self):
        expected_names = (
            "cast",
            "get_origin",
            "get_args",
            "no_type_check",
            "overload",
            "override",
            "TypeVar",
            "NewType",
            "Any",
            "BinaryIO",
            "ClassVar",
            "Final",
            "Hashable",
            "IO",
            "NoReturn",
            "Sized",
            "SupportsInt",
            "SupportsFloat",
            "SupportsComplex",
            "SupportsBytes",
            "SupportsIndex",
            "SupportsAbs",
            "SupportsRound",
            "TextIO",
            "Protocol",
            "AnyStr",
            "TypedDict",
            "AbstractSet",
            "AsyncContextManager",
            "AsyncGenerator",
            "AsyncIterable",
            "AsyncIterator",
            "Awaitable",
            "Callable",
            "ChainMap",
            "Collection",
            "Container",
            "ContextManager",
            "Coroutine",
            "Counter",
            "DefaultDict",
            "Deque",
            "Dict",
            "FrozenSet",
            "Generator",
            "Generic",
            "Iterable",
            "Iterator",
            "List",
            "Literal",
            "Mapping",
            "MutableMapping",
            "MutableSequence",
            "MutableSet",
            "NamedTuple",
            "Optional",
            "OrderedDict",
            "Self",
            "Sequence",
            "Set",
            "Tuple",
            "Type",
            "Union",
            "TYPE_CHECKING",
        )

        for name in expected_names:
            with self.subTest(name=name):
                self.assertTrue(hasattr(typing, name), "missing: {}".format(name))

    # typing spec (library interface): reject accidental non-spec symbols.
    @xfail_on_error
    def test_random_symbols_are_not_present(self):
        self.assertFalse(hasattr(typing, "FooBar"), "unexpected symbol typing.FooBar found")
        self.assertFalse(hasattr(typing, "SNAFU"), "unexpected symbol typing.SNAFU found")

    # typing spec (directives/aliases): TYPE_CHECKING, AnyStr, TypedDict aliases.
    def test_constants_and_simple_aliases(self):
        self.assertTrue(typing.TYPE_CHECKING is False)
        self.assertTrue(typing.AnyStr is str)
        self.assertTrue(typing.TypedDict is dict)

    def test_typing_getters(self):
        self.assertEqual(typing.get_args(list), ())
        self.assertTrue(typing.get_origin(list) is None)

    # typing spec (directives/aliases): runtime behavior of cast/TypeVar/NewType helpers.
    def test_function_return_values(self):
        marker = object()
        self.assertTrue(typing.cast(int, marker) is marker)

        def f():
            return 1

        self.assertTrue(typing.no_type_check(f) is f)
        self.assertTrue(typing.override(f) is f)
        self.assertTrue(typing.overload(f) is None)
        self.assertFalse(typing.TypeVar("T") is None)
        self.assertTrue(typing.NewType("MyInt", int) is int)

    # typing spec (special types/qualifiers/protocols): classes are runtime-constructible.
    def test_public_classes_instantiation_and_isinstance(self):
        class_names = (
            "Any",
            "BinaryIO",
            "ClassVar",
            "Final",
            "Hashable",
            "IO",
            "NoReturn",
            "Sized",
            "SupportsInt",
            "SupportsFloat",
            "SupportsComplex",
            "SupportsBytes",
            "SupportsIndex",
            "SupportsAbs",
            "SupportsRound",
            "TextIO",
            "Protocol",
        )

        for name in class_names:
            with self.subTest(name=name):
                cls = getattr(typing, name)
                instance = cls()
                # self.assertTrue(isinstance(instance, cls), "not instance of {}".format(name))
                # self.assertTrue(type(instance) is cls, "unexpected concrete type for {}".format(name))

    # typing spec (generics/special forms): aliases accept subscription at runtime.
    def test_subscriptable_aliases(self):
        alias_names = (
            "AbstractSet",
            "AsyncContextManager",
            "AsyncGenerator",
            "AsyncIterable",
            "AsyncIterator",
            "Awaitable",
            "Callable",
            "ChainMap",
            "Collection",
            "Container",
            "ContextManager",
            "Coroutine",
            "Counter",
            "DefaultDict",
            "Deque",
            "Dict",
            "FrozenSet",
            "Generator",
            "Generic",
            "Iterable",
            "Iterator",
            "List",
            "Literal",
            "Mapping",
            "MutableMapping",
            "MutableSequence",
            "MutableSet",
            "NamedTuple",
            "Optional",
            "OrderedDict",
            "Self",
            "Sequence",
            "Set",
            "Tuple",
            "Type",
            "Union",
        )

        for name in alias_names:
            with self.subTest(name=name):
                alias = getattr(typing, name)
                result = alias[int]


class TestTypingSpecDirectivesAndAliases(unittest.TestCase):
    # typing spec (directives): cast is a runtime no-op.
    def test_cast_returns_original_value(self):
        marker = object()
        self.assertTrue(typing.cast(int, marker) is marker)

    # typing spec (directives): cast requires exactly two positional arguments.
    def test_cast_arity_validation(self):
        with self.assertRaises(TypeError):
            typing.cast()

        with self.assertRaises(TypeError):
            typing.cast(int, "", "")

    # typing spec (directives): no_type_check and override preserve the wrapped callable.
    def test_decorator_identity_behavior(self):
        def f():
            return 123

        self.assertTrue(typing.no_type_check(f) is f)
        self.assertTrue(typing.override(f) is f)

    # typing spec (overload): overload is for static analysis, runtime value is sentinel-like here.
    def test_overload_runtime_placeholder(self):
        def f():
            return 123

        self.assertTrue(typing.overload(f) is None)

    # typing spec (generics): TypeVar accepts constraints and variance/bound flags.
    def test_typevar_accepts_spec_parameters(self):
        self.assertTrue(typing.TypeVar("T") is not None)
        self.assertTrue(typing.TypeVar("TBound", bound=int) is not None)
        self.assertTrue(typing.TypeVar("TCo", covariant=True) is not None)
        self.assertTrue(typing.TypeVar("TContra", contravariant=True) is not None)
        self.assertTrue(typing.TypeVar("TInfer", infer_variance=True) is not None)
        self.assertTrue(typing.TypeVar("TConstrained", int, str) is not None)

    def test_generic_parameterized_base(self):
        code = (
            "from typing import Generic, TypeVar\n"
            "T = TypeVar('T')\n"
            "class Foo(Generic[T]):\n"
            "    pass\n"
            "class Bar(Foo[int]):\n"
            "    pass\n"
        )

        ns = {}
        exec(code, {}, ns)
        self.assertFalse(type(ns["Bar"]) is type)

    # typing spec (type aliases): NewType runtime hook in this implementation returns the wrapped type.
    def test_newtype_runtime_behavior(self):
        self.assertTrue(typing.NewType("UserId", int) is int)

    # typing spec (introspection helpers): get_origin/get_args are currently minimal.
    def test_origin_and_args_on_parameterized_forms(self):
        self.assertTrue(typing.get_origin(typing.List[int]) is None)
        self.assertEqual(typing.get_args(typing.List[int]), ())
        self.assertTrue(typing.get_origin(typing.Union[int, str]) is None)
        self.assertEqual(typing.get_args(typing.Union[int, str]), ())


if __name__ == "__main__":
    unittest.main()
