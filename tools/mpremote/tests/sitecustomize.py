"""
Subprocess coverage support for mpremote tests.

This file is automatically loaded by Python when it's in PYTHONPATH.
It starts coverage collection for subprocesses when COVERAGE_PROCESS_START is set.

The coverage.process_startup() function:
- Checks if COVERAGE_PROCESS_START is set to a coverage config file
- If set, starts coverage collection for this subprocess
- Coverage data is written to .coverage.{hostname}.{pid}.{random} files
- These files are combined with `coverage combine` after tests run

For debugging, set MPREMOTE_COVERAGE_DEBUG=1 to see when coverage starts.

Context Passing:
- Set MPREMOTE_COVERAGE_CONTEXT to pass the test context to subprocesses
- Format: "[scenario]test_path" e.g. "[socket]tests/test_repl.py::TestClass::test_name|run"
- The subprocess will use this as its coverage context
"""

import os
import sys


def _start_subprocess_coverage():
    """Start coverage in subprocess if COVERAGE_PROCESS_START is configured."""
    config_file = os.environ.get("COVERAGE_PROCESS_START", "")
    debug = os.environ.get("MPREMOTE_COVERAGE_DEBUG", "")
    context = os.environ.get("MPREMOTE_COVERAGE_CONTEXT", "")

    if not config_file:
        if debug:
            print(
                f"[sitecustomize] COVERAGE_PROCESS_START not set, skipping coverage",
                file=sys.stderr,
            )
        return

    if not os.path.exists(config_file):
        if debug:
            print(f"[sitecustomize] Config file not found: {config_file}", file=sys.stderr)
        return

    try:
        import coverage

        # Start coverage with the context from the parent test process
        cov = coverage.Coverage(config_file=config_file)
        cov.start()

        # If a context was passed from the test, switch to it
        if context:
            cov.switch_context(context)
            if debug:
                print(
                    f"[sitecustomize] Coverage started with context '{context}' (pid={os.getpid()})",
                    file=sys.stderr,
                )
        else:
            if debug:
                print(
                    f"[sitecustomize] Coverage started (pid={os.getpid()}, no context)",
                    file=sys.stderr,
                )

        # Register atexit handler to save coverage data
        import atexit

        atexit.register(_save_coverage, cov, debug)

    except ImportError:
        if debug:
            print(f"[sitecustomize] Coverage module not installed", file=sys.stderr)
    except Exception as e:
        if debug:
            print(f"[sitecustomize] Failed to start coverage: {e}", file=sys.stderr)


def _save_coverage(cov, debug):
    """Save coverage data when subprocess exits."""
    try:
        cov.stop()
        cov.save()
        if debug:
            print(f"[sitecustomize] Coverage saved (pid={os.getpid()})", file=sys.stderr)
    except Exception as e:
        if debug:
            print(f"[sitecustomize] Failed to save coverage: {e}", file=sys.stderr)


_start_subprocess_coverage()
