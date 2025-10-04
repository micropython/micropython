import __future__

from pathlib import Path

from docs_test.mp_evaluator import MicroPythonDocTestEvaluator, MicroPythonEvaluator
from docs_test.mp_parser import (
    MicroPythonClearNamespaceParser,
    MicroPythonDocTestParser,
    MicroPythonSkipParser,
)
from sybil import Sybil
from sybil.integration.pytest import SybilItem as OriginalSybilItem
from sybil.parsers.rest import CodeBlockParser, DocTestDirectiveParser, PythonCodeBlockParser


# Monkeypatch SybilItem.reportinfo to return 0-based line numbers as pytest expects
# In conftest.py near top
def _patch_sybil_line_numbers():
    from sybil.integration.pytest import SybilItem as _SybilItem

    if getattr(_SybilItem.reportinfo, "_mp_patched", False):
        return
    original = _SybilItem.reportinfo

    def patched(self):
        path, _, info = original(self)
        # original returns 1-based; convert to 0-based for pytest
        return path, self.example.line - 1, info

    patched._mp_patched = True  # type: ignore[attr-defined]
    _SybilItem.reportinfo = patched


_patch_sybil_line_numbers()

# docs_path = Path(__file__).parent / "examples"
# docs_path = Path(__file__).parent / "micropython/docs"
docs_path = Path(__file__).parent

mp_code = Sybil(
    path=docs_path.as_posix(),
    parsers=[
        MicroPythonSkipParser(),  # MCU-aware skip parser
        MicroPythonClearNamespaceParser(),  # Clear namespace between logical blocks
        MicroPythonDocTestParser(),  # Custom doctest & directive parser
        CodeBlockParser(
            language="python", evaluator=MicroPythonEvaluator()
        ),  # Standard code block evaluator
    ],
    pattern="*.rst",
)


pytest_collect_file = (mp_code).pytest()
