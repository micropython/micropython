# CPython vs MicroPython Differences Tests

This folder contains test files that document and verify the differences between 
CPython and MicroPython implementations.

## Purpose

These tests are designed to:
- Execute the same code on both CPython and MicroPython
- Document behavioral differences between the two implementations
- Generate documentation pages that help users understand these differences

## How It Works

1. Each test file contains Python code that demonstrates a specific difference
2. The tests are executed on both CPython and MicroPython
3. The output from both implementations is captured and compared
4. The results, along with metadata from the file docstrings, are used to generate documentation

## Documentation Generation

The documentation is automatically generated using:
```
tools/gen-cpydiff.py
```

This script:
- Parses the docstring metadata from each test file
- Runs the tests on both implementations
- Combines the results to create comprehensive documentation pages
- Outputs formatted reStructuredText documentation showing the differences

**Note:** This script is automatically executed as part of the documentation publishing
process when building the docs (via the docs Makefile for both HTML and LaTeX output).

## Test File Format

Each test file should include a docstring with the following format:
```python
"""
categories: Category1,Category2
description: Brief description of the difference being tested
cause: Explanation of why this difference exists
workaround: How to work around this difference (or "Unknown" if none)
"""
```

The categories are used to organize the documentation into sections. Common categories include:
- Syntax
- Core (Core language)
- Types (Builtin types)
- Modules

## Example Test File

An example file demonstrating a syntax difference:

```python
"""
categories: Syntax,Operators
description: MicroPython allows := to assign to the iteration variable in nested comprehensions, CPython does not.
cause: MicroPython is optimised for code size.
workaround: Do not use := to assign to the iteration variable of a comprehension.
"""

print([[(j := i) for i in range(2)] for j in range(2)])
```

This example will:
- Run on both CPython (which will produce a syntax error) and MicroPython (which will execute successfully)
- Generate documentation showing the different outputs
- Include the cause and workaround information in the generated docs

## Usage

The documentation is automatically regenerated during the documentation build process.
To manually regenerate after adding or modifying tests:

1. Set environment variables if needed:
   - `MICROPY_MICROPYTHON`: Path to MicroPython executable
   - `MICROPY_CPYTHON3`: Path to CPython 3.x executable (default: "python3")

2. Run the generation script from the project root:
   ```
   python tools/gen-cpydiff.py
   ```

The generated documentation will be placed in `docs/genrst/` as reStructuredText files.

Also see docs/README.md for more information on building the documentation locally 
to validate the rendering of the resulting documentation page(s).
