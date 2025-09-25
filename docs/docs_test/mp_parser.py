import __future__

import re
from doctest import DocTestParser as StdDocTestParser

from .mp_evaluator import MicroPythonDocTestEvaluator, MicroPythonEvaluator
from .mp_runner import reset_micropython_mcu, run_micropython_code
from sybil import Example, Region
from sybil.parsers.abstract.doctest import DocTestStringParser
from sybil.parsers.rest import DocTestParser


class MicroPythonDocTestStringParser(DocTestStringParser):
    """
    Custom doctest string parser that avoids conflicts with code blocks
    and runs doctests on MicroPython MCU
    """

    def __call__(self, string: str, name: str):
        """Override the callable interface to use our custom parse method"""
        return self.parse(string, name)

    def parse(self, text: str, path: str):
        """Parse text and yield regions, handling both inline doctests and doctest directives"""

        lines = text.split("\n")

        # Find all code block regions AND skip regions to avoid conflicts
        exclusion_regions = []
        in_code_block = False
        code_block_start = None
        in_skip_region = False
        skip_region_start = None

        for line_idx, line in enumerate(lines):
            stripped = line.strip()

            # Handle code blocks
            if stripped.startswith(".. code-block::"):
                in_code_block = True
                code_block_start = line_idx
            elif in_code_block and line and not line.startswith(" ") and not line.startswith("\t"):
                # End of code block (when we hit non-indented content)
                if code_block_start is not None:
                    exclusion_regions.append((code_block_start, line_idx))
                in_code_block = False
                code_block_start = None

            # Handle skip regions (start/end pairs)
            elif stripped.startswith(".. skip:") and "start" in stripped:
                # Begin skip region
                in_skip_region = True
                skip_region_start = line_idx
            elif stripped.startswith(".. skip:") and "end" in stripped:
                # End skip region
                if in_skip_region and skip_region_start is not None:
                    exclusion_regions.append(
                        (skip_region_start, line_idx + 1)
                    )  # Include the end line
                in_skip_region = False
                skip_region_start = None

            # Handle other skip directives (like .. skip: next) that are single-directive exclusions
            elif stripped.startswith(".. skip:") and not in_skip_region:
                # Single skip directive - exclude until next non-indented content
                single_skip_start = line_idx
                # Look ahead to find the end of this single skip directive
                j = line_idx + 1
                while j < len(lines):
                    next_line = lines[j]
                    if (
                        next_line
                        and not next_line.startswith(" ")
                        and not next_line.startswith("\t")
                    ):
                        exclusion_regions.append((single_skip_start, j))
                        break
                    j += 1
                else:
                    # Skip goes to end of file
                    exclusion_regions.append((single_skip_start, len(lines)))

        # Handle case where code block goes to end of file
        if in_code_block and code_block_start is not None:
            exclusion_regions.append((code_block_start, len(lines)))

        # Handle case where skip region goes to end of file without proper end
        if in_skip_region and skip_region_start is not None:
            exclusion_regions.append((skip_region_start, len(lines)))

        # Now parse for doctest content, avoiding exclusion regions
        i = 0
        while i < len(lines):
            line = lines[i]
            stripped = line.strip()

            # Check if we're inside an exclusion region
            inside_exclusion = any(start <= i < end for start, end in exclusion_regions)
            if inside_exclusion:
                i += 1
                continue

            # Handle doctest directives
            if stripped == ".. doctest::":
                start_line = i
                i += 1  # Skip the directive line

                # Skip any options/arguments lines
                while i < len(lines) and (
                    not lines[i].strip() or lines[i].startswith(" :") or lines[i].startswith("\t:")
                ):
                    i += 1

                # Collect indented doctest content
                doctest_lines = []
                while i < len(lines):
                    current_line = lines[i]

                    # Check if we're entering an exclusion region
                    inside_exclusion = any(start <= i < end for start, end in exclusion_regions)
                    if inside_exclusion:
                        break

                    # If line is indented (part of the directive content)
                    if (
                        current_line.startswith(" ")
                        or current_line.startswith("\t")
                        or not current_line.strip()
                    ):
                        if (
                            current_line.strip().startswith(">>> ")
                            or current_line.strip().startswith("... ")
                            or not current_line.strip()
                        ):
                            doctest_lines.append(current_line)
                        elif doctest_lines:  # Expected output after doctest commands
                            doctest_lines.append(current_line)
                        i += 1
                    else:
                        # End of indented block
                        break

                # Process the doctest directive content
                if doctest_lines:
                    # Remove common indentation
                    import textwrap

                    doctest_text = textwrap.dedent("\n".join(doctest_lines)).strip()

                    if doctest_text:
                        # Use standard doctest parser to extract examples
                        std_parser = StdDocTestParser()
                        examples = std_parser.get_examples(doctest_text, path)

                        if examples:
                            # Calculate character positions for the entire directive region
                            start_pos = sum(
                                len(lines[j]) + 1 for j in range(start_line)
                            )  # +1 for newline
                            end_line = start_line + len([lines[start_line]]) + len(doctest_lines)
                            end_pos = sum(
                                len(lines[j]) + 1 for j in range(min(end_line, len(lines)))
                            )

                            # Create one region for all examples in this directive
                            yield Region(
                                start=start_pos,
                                end=end_pos,
                                parsed=examples,
                                evaluator=self.evaluator,
                            )

            # Handle inline doctests (outside of directives)
            elif stripped.startswith(">>> "):
                # FIRST check if we're inside an exclusion region before processing
                inside_exclusion = any(start <= i < end for start, end in exclusion_regions)
                if inside_exclusion:
                    i += 1
                    continue

                start_line = i
                doctest_lines = []

                # Collect the doctest block
                while i < len(lines):
                    current_line = lines[i]
                    stripped = current_line.strip()

                    # Check if we're entering an exclusion region
                    inside_exclusion = any(start <= i < end for start, end in exclusion_regions)
                    if inside_exclusion:
                        break

                    if stripped.startswith(">>> ") or stripped.startswith("... "):
                        doctest_lines.append(current_line)
                        i += 1
                    elif not stripped:  # Empty line
                        # Check if next non-empty line is expected output or another construct
                        peek_idx = i + 1
                        while peek_idx < len(lines) and not lines[peek_idx].strip():
                            peek_idx += 1

                        if peek_idx < len(lines):
                            peek_line = lines[peek_idx].strip()
                            # If it's another doctest or RST directive, stop here
                            if peek_line.startswith(">>> ") or peek_line.startswith(".. "):
                                break
                            # Otherwise, include expected output
                            else:
                                # Include the empty line and continue to expected output
                                doctest_lines.append(current_line)
                                i += 1
                        else:
                            break
                    elif not (stripped.startswith(">>>") or stripped.startswith(".. ")):
                        # This is expected output, include it
                        doctest_lines.append(current_line)
                        i += 1
                    else:
                        # Hit another construct, stop
                        break

                # Process the collected inline doctest block
                if doctest_lines:
                    doctest_text = "\n".join(doctest_lines)
                    # Use standard doctest parser to extract examples
                    std_parser = StdDocTestParser()
                    examples = std_parser.get_examples(doctest_text, path)

                    if examples:
                        # Calculate character positions
                        start_pos = sum(
                            len(lines[j]) + 1 for j in range(start_line)
                        )  # +1 for newline
                        end_pos = start_pos + len(doctest_text)

                        # Create one region for all examples in this doctest block
                        yield Region(
                            start=start_pos, end=end_pos, parsed=examples, evaluator=self.evaluator
                        )
            else:
                i += 1


class MicroPythonDocTestParser(DocTestParser):
    """
    MicroPython-aware doctest parser that subclasses Sybil's DocTestParser
    """

    def __init__(self, optionflags: int = 0):
        # Don't call super().__init__ because we want to use our custom string parser
        self.string_parser = MicroPythonDocTestStringParser(
            evaluator=MicroPythonDocTestEvaluator()
        )


class MicroPythonSkipper:
    """
    MCU-aware version of Sybil's Skipper that evaluates conditions on MicroPython MCU
    """

    def __init__(self, directive: str) -> None:
        self.document_state = {}
        self.directive = directive

    def state_for(self, example):
        document = example.document
        if document not in self.document_state:
            from sybil.evaluators.skip import SkipState

            self.document_state[document] = SkipState()
        return self.document_state[example.document]

    def install(self, example, state, reason):
        document = example.document
        document.push_evaluator(self)
        if reason:
            reason = reason.lstrip()
            if reason.startswith("if"):
                condition = reason[2:].strip()
                # Evaluate condition on MCU instead of host
                should_skip = self._evaluate_condition_on_mcu(condition)
                if should_skip:
                    from unittest import SkipTest

                    state.exception = SkipTest("MCU condition evaluated to True")
                else:
                    state.active = False
            else:
                # Handle quoted reasons (non-conditional) - just skip with the quoted reason
                from unittest import SkipTest

                state.exception = SkipTest(reason.strip('"'))
        else:
            # Simple skip directive without reason - always skip
            from unittest import SkipTest

            state.exception = SkipTest("Skipped by directive")

    def remove(self, example):
        document = example.document
        document.pop_evaluator(self)
        del self.document_state[document]

    def evaluate_skip_example(self, example):
        state = self.state_for(example)
        directive = self.directive
        action, reason = example.parsed

        if action not in ("start", "next", "end"):
            raise ValueError("Bad skip action: " + action)
        if state.last_action is None and action not in ("start", "next"):
            raise ValueError(f"'{directive}: {action}' must follow '{directive}: start'")
        elif state.last_action and action != "end":
            raise ValueError(
                f"'{directive}: {action}' cannot follow '{directive}: {state.last_action}'"
            )

        state.last_action = action

        if action == "start":
            self.install(example, state, reason)
        elif action == "next":
            self.install(example, state, reason)
            state.remove = True
        elif action == "end":
            self.remove(example)
            if reason:
                raise ValueError("Cannot have condition on 'skip: end'")

    def evaluate_other_example(self, example):
        state = self.state_for(example)
        if state.remove:
            self.remove(example)
        if not state.active:
            from sybil.example import NotEvaluated

            raise NotEvaluated()
        if state.exception is not None:
            raise state.exception

    def __call__(self, example):
        if example.region.evaluator is self:
            self.evaluate_skip_example(example)
        else:
            self.evaluate_other_example(example)

    def _evaluate_condition_on_mcu(self, condition: str) -> bool:
        """Evaluate a condition on the MicroPython MCU and return True if should skip"""
        try:
            # Create Python code that evaluates the condition and returns the result
            test_code = f"""
try:
    result = {condition}
    print("SKIP_CONDITION_RESULT:", result)
except Exception as e:
    print("SKIP_CONDITION_ERROR:", str(e))
"""
            output = run_micropython_code(test_code)
            print(f"MCU Skip evaluation output: {output.strip()}")

            # Parse the output
            for line in output.split("\n"):
                if line.startswith("SKIP_CONDITION_RESULT:"):
                    result_str = line.split("SKIP_CONDITION_RESULT:", 1)[1].strip()
                    # Convert string representation back to boolean
                    result = result_str.lower() == "true"
                    print(f"Skip condition '{condition}' evaluated to: {result}")
                    return result
                elif line.startswith("SKIP_CONDITION_ERROR:"):
                    # If there's an error, don't skip (default behavior)
                    print(f"Skip condition error: {line}")
                    return False

            return False  # Default: don't skip if we can't determine the result

        except Exception as e:
            print(f"Error evaluating skip condition on MCU: {e}")
            return False  # Default: don't skip on error


class MicroPythonSkipParser:
    """
    MCU-aware skip parser that subclasses the original SkipParser behavior
    but evaluates conditions on the MicroPython MCU
    """

    def __init__(self):
        from sybil.parsers.rest import SkipParser

        # Create a standard SkipParser and copy its behavior
        self._standard_parser = SkipParser()
        # But use our MCU-aware skipper
        self.skipper = MicroPythonSkipper("skip")
        # Copy the lexers from the standard parser
        self.lexers = self._standard_parser.lexers
        self.directive = "skip"

    def __call__(self, document):
        """Parse document using standard SkipParser logic but with MCU evaluation"""
        for lexed in self.lexers(document):
            arguments = lexed.lexemes["arguments"]
            if arguments is None:
                raise ValueError(f"missing arguments to {self.directive}")

            from sybil.parsers.abstract.skip import SKIP_ARGUMENTS_PATTERN

            match = SKIP_ARGUMENTS_PATTERN.match(arguments)
            if match is None:
                raise ValueError(f"malformed arguments to {self.directive}: {arguments!r}")

            yield Region(lexed.start, lexed.end, match.groups(), self.skipper)


class MicroPythonClearNamespaceParser:
    """
    Parser for clear-namespace directives that resets the MicroPython MCU
    """

    def __call__(self, document):
        """Parse document and yield regions for clear-namespace directives"""
        # Look for clear-namespace directive (with or without ::)
        pattern = r"^(?P<indent> *)\.\.[ \t]+clear-namespace[ \t]*(?:::)?[ \t]*$"

        for match in re.finditer(pattern, document.text, re.MULTILINE):
            yield Region(
                start=match.start(),
                end=match.end(),
                parsed=None,  # No parsing needed
                evaluator=self._clear_namespace_evaluator,
            )

    def _clear_namespace_evaluator(self, example: Example) -> None:
        """Reset the MicroPython MCU to clear the namespace"""
        reset_micropython_mcu()
