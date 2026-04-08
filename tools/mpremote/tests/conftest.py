"""
Pytest configuration for mpremote tests with scenario-aware coverage.

Configures coverage contexts based on test scenario (device type) so
the coverage report shows which devices covered each line.

Usage:
    MPREMOTE_DEVICES=socket://localhost:2218,rfc2217://localhost:2217 \
        pytest tests/ --cov=mpremote --cov-context=test -v
    coverage html

Coverage contexts: [socket]test_path, [rfc2217]test_path, [unittest]test_path
"""

import os
from pathlib import Path

import pytest
from shared_utils import get_scenario_from_device

# Module-level paths for subprocess coverage
_TESTS_DIR = Path(__file__).parent.absolute()
_MPREMOTE_DIR = _TESTS_DIR.parent
_CONFIG_FILE = _MPREMOTE_DIR / "pyproject.toml"


def get_device_scenario():
    """
    Determine the test scenario from the MPREMOTE_DEVICE environment variable.

    This is used for legacy tests that don't use parametrize.
    """
    device = os.environ.get("MPREMOTE_DEVICE", "")
    return get_scenario_from_device(device)


def get_scenario_from_test(item):
    """
    Extract scenario from a test item's parameters or markers.

    Checks for:
    1. 'scenario' parameter from parametrize
    2. Test name suffix like [socket] or [rfc2217]
    3. Falls back to environment variable
    """
    # Check if test has 'scenario' parameter from parametrize
    if hasattr(item, "callspec") and "scenario" in item.callspec.params:
        return item.callspec.params["scenario"]

    # Check test name for parametrize suffix like [socket]
    if "[" in item.name and "]" in item.name:
        # Extract parameter from test name, e.g., "test_foo[socket]" -> "socket"
        suffix = item.name.split("[")[-1].rstrip("]")
        # Could be "socket" or "socket-rfc2217" etc.
        return suffix.split("-")[0] if "-" in suffix else suffix

    # Fall back to environment variable
    return get_device_scenario()


# Store scenario at module level for non-parametrized tests
_SCENARIO = get_device_scenario()
_DEVICE = os.environ.get("MPREMOTE_DEVICE", "")


def pytest_report_header(config):
    """Add device information to the pytest header."""
    devices_str = os.environ.get("MPREMOTE_DEVICES", "")
    device_str = os.environ.get("MPREMOTE_DEVICE", "")

    if devices_str:
        devices = [d.strip() for d in devices_str.split(",") if d.strip()]
        return [
            f"mpremote devices: {', '.join(devices)}",
            f"coverage contexts: {', '.join(get_scenario_from_device(d) for d in devices)}",
        ]
    elif device_str:
        return [
            f"mpremote device: {device_str}",
            f"coverage scenario: {get_scenario_from_device(device_str)}",
        ]
    else:
        return ["mpremote: no devices configured (set MPREMOTE_DEVICES or MPREMOTE_DEVICE)"]


@pytest.fixture(scope="session")
def device_scenario():
    """Fixture that provides the current test scenario name."""
    return _SCENARIO


@pytest.fixture(scope="session") 
def device():
    """Fixture that provides the MPREMOTE_DEVICE value."""
    return _DEVICE


# Cache devices at session level - only computed once
_CACHED_DEVICES = None


def _get_cached_devices():
    """Get devices list, cached for the session."""
    global _CACHED_DEVICES
    if _CACHED_DEVICES is None:
        from shared_utils import get_devices

        _CACHED_DEVICES = get_devices()
    return _CACHED_DEVICES


@pytest.fixture(scope="session")
def devices():
    """
    Session-scoped fixture providing the list of configured devices.

    Returns list of (device_url, scenario_name) tuples.
    """
    return _get_cached_devices()


def pytest_generate_tests(metafunc):
    """
    Dynamically parametrize tests that request 'device' and 'scenario' parameters.

    This is the pytest-native way to do dynamic parametrization at collection time.
    Tests can simply declare (device, scenario) as parameters and this hook will
    populate them from MPREMOTE_DEVICES environment variable.

    If no devices are configured, tests are skipped with a clear message.
    """
    # Only parametrize if test requests both device and scenario
    if "device" in metafunc.fixturenames and "scenario" in metafunc.fixturenames:
        devices = _get_cached_devices()

        if devices:
            metafunc.parametrize(
                "device,scenario",
                devices,
                ids=[d[1] for d in devices],  # Use scenario as test ID
            )
        else:
            # No devices configured - add a skipped placeholder
            metafunc.parametrize(
                "device,scenario",
                [
                    pytest.param(
                        "", "no-device", marks=pytest.mark.skip(reason="No devices configured")
                    )
                ],
                ids=["no-device"],
            )


class ScenarioPrefixPlugin:
    """
    Plugin to prefix coverage context with the scenario name.

    This creates contexts like '[rfc2217]tests/test_repl_newlines.py::TestClass::test_name|run'

    For parametrized tests with device parameter, the scenario is extracted from the device.
    For unit tests (no device parameter), the scenario is 'unittest'.

    We only modify _nodeid (used for coverage context), not 'name' (used for display).
    """

    def __init__(self, default_scenario):
        self.default_scenario = default_scenario

    @pytest.hookimpl(hookwrapper=True)
    def pytest_runtest_protocol(self, item, nextitem):
        """
        Prefix the test's node ID with the scenario for coverage context.

        Creates coverage contexts like:
          '[rfc2217]tests/test_repl_newlines.py::TestClass::test_name|run'
          '[unittest]tests/test_repl_newlines.py::TestClass::test_name|run'

        Only modifies _nodeid (coverage context), not name (display name).
        """
        original_nodeid = item.nodeid

        # Get scenario from test parameters or fall back to default
        scenario = get_scenario_from_test(item)

        # Prefix scenario to the full test path
        item._nodeid = f"[{scenario}]{original_nodeid}"

        try:
            yield
        finally:
            # Restore original nodeid
            item._nodeid = original_nodeid


def pytest_configure(config):
    """Register plugins, configure scenario, and set up subprocess coverage."""
    # Register custom marker
    config.addinivalue_line("markers", "scenario(name): mark test with a device scenario")

    global _SCENARIO, _DEVICE
    _SCENARIO = get_device_scenario()
    _DEVICE = os.environ.get("MPREMOTE_DEVICE", "")
    config._mpremote_scenario = _SCENARIO
    config._mpremote_device = _DEVICE

    # Register the scenario prefix plugin
    config.pluginmanager.register(ScenarioPrefixPlugin(_SCENARIO), "scenario_prefix")

    # Set up subprocess coverage environment variables
    # These will be inherited by subprocesses spawned during tests
    _setup_subprocess_coverage(config)

    # Print scenario banner
    print(f"\n📡 Test scenario: {_SCENARIO} ({_DEVICE or 'no device'})")


def _setup_subprocess_coverage(config):
    """
    Configure environment for subprocess coverage collection.

    Sets COVERAGE_PROCESS_START so that test helpers can detect when coverage
    is active and wrap subprocess calls with `coverage run --context=...`.

    The actual coverage wrapping is done by get_spawn_command() in the test files,
    using the same pattern as the bash tests in run-mpremote-tests.sh.
    """
    # Only set up if we're collecting coverage
    if not hasattr(config, "_cov"):
        # Check if pytest-cov is being used
        cov_plugin = config.pluginmanager.get_plugin("_cov")
        if cov_plugin is None:
            return

    # Set COVERAGE_PROCESS_START to our config file
    if _CONFIG_FILE.exists():
        os.environ["COVERAGE_PROCESS_START"] = str(_CONFIG_FILE)

        # Store for validation later
        config._subprocess_coverage_enabled = True
        config._subprocess_coverage_dir = str(_MPREMOTE_DIR)
    else:
        config._subprocess_coverage_enabled = False


def pytest_collection_modifyitems(config, items):
    """Add scenario marker and properties to all tests."""
    for item in items:
        scenario = get_scenario_from_test(item)
        item.add_marker(pytest.mark.scenario(scenario))
        item.user_properties.append(("scenario", scenario))


def pytest_terminal_summary(terminalreporter, exitstatus, config):
    """Add coverage scenario summary to terminal output."""
    terminalreporter.write_sep("=", "Coverage Scenario Summary")
    terminalreporter.write_line("")
    _validate_subprocess_coverage(terminalreporter, config)


def _validate_subprocess_coverage(terminalreporter, config):
    """Check if subprocess coverage was collected and report status."""
    if not getattr(config, "_subprocess_coverage_enabled", False):
        terminalreporter.write_line("ℹ️  Coverage not enabled (use --cov or check pyproject.toml)")
        return

    coverage_dir = getattr(config, "_subprocess_coverage_dir", str(_MPREMOTE_DIR))
    main_coverage = os.path.join(coverage_dir, ".coverage")

    # Check for device scenario contexts in coverage database
    device_contexts = []
    if os.path.exists(main_coverage):
        try:
            import sqlite3

            conn = sqlite3.connect(main_coverage)
            cursor = conn.execute("SELECT context FROM context WHERE context != ''")
            for row in cursor:
                ctx = row[0]
                for scenario in ["socket", "rfc2217", "ttyACM", "ttyUSB", "COM"]:
                    if f"[{scenario}" in ctx:
                        device_contexts.append(ctx)
                        break
            conn.close()
        except Exception:
            pass

    if device_contexts:
        scenarios = {ctx.split("]")[0][1:] for ctx in device_contexts if ctx.startswith("[")}
        terminalreporter.write_line(
            f"✅ Subprocess coverage: {len(device_contexts)} device context(s)"
        )
        if scenarios:
            terminalreporter.write_line(f"   Scenarios: {', '.join(sorted(scenarios))}")
    else:
        terminalreporter.write_line("ℹ️  No device coverage (only unit tests ran)")
    terminalreporter.write_line("")
