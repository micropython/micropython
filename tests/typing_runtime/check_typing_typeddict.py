# TypedDict runtime parity checks from the notebook scenarios.

try:
    from typing import TypedDict
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest
from typing import TypedDict, TypeVar


class TestTypingTypedDictRuntime(unittest.TestCase):
    # Basic TypedDict-like construction path.
    def test_typed_dict_construction_and_access(self):
        class Movie(TypedDict):
            name: str
            year: int

        movie = Movie(name="Blade Runner", year=1982)
        self.assertTrue(isinstance(movie, dict))
        self.assertEqual(movie["name"], "Blade Runner")
        self.assertEqual(movie["year"], 1982)

    @unittest.expectedFailure
    def test_typed_dict_isinstance_runtime_cpydiff(self):
        class Movie(TypedDict):
            name: str
            year: int

        movie = {"name": "Blade Runner", "year": 1982}
        with self.assertRaises(TypeError):
            isinstance(movie, Movie)
            # TODO: document using cpy-diff
            # CPython : raise TypeError('TypedDict does not support instance and class checks')
            # MicroPython: No error : isinstance(movie, Movie) returns True

    # Example of this static-analysis pattern; runtime call path should still execute.
    def test_typevar_bound_typed_dict_runtime_path(self):
        T = TypeVar("T", bound=TypedDict)
        self.assertTrue(T is None or hasattr(T, "__class__"))


if __name__ == "__main__":
    unittest.main()
