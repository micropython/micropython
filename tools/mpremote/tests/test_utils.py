"""
Shared utilities for mpremote tests.

This module contains helper functions used by both pytest tests and
can be imported directly (unlike conftest.py which is pytest-specific).
"""

import os
import sys


def get_scenario_from_device(device):
    """
    Determine the scenario name from a device URL.

    Returns a human-readable scenario name like:
    - "socket" for socket://... connections
    - "rfc2217" for rfc2217://... connections
    - "ttyACM0" for /dev/ttyACM0
    - "COM3" for COM3
    - "unittest" for tests without a device
    """
    if not device:
        return "unittest"
    elif device.startswith("socket://"):
        return "socket"
    elif device.startswith("rfc2217://"):
        return "rfc2217"
    elif device.startswith("/dev/"):
        # Extract device name (e.g., ttyACM0 from /dev/ttyACM0)
        return device.split("/")[-1]
    elif device.startswith("COM"):
        return device
    else:
        return "other"


def get_mpremote_path():
    """Get the path to mpremote.py."""
    test_dir = os.path.dirname(os.path.abspath(__file__))
    return os.path.join(test_dir, "..", "mpremote.py")


def get_spawn_command(mpremote_path, mpremote_args, context=None):
    """
    Get command and args for spawning mpremote, with optional coverage wrapping.

    When coverage is active (COVERAGE_PROCESS_START is set), wraps the command
    with `coverage run --context=...` - the same pattern used by bash tests.

    Args:
        mpremote_path: Path to mpremote.py
        mpremote_args: List of args to pass to mpremote (e.g., ["connect", device])
        context: Coverage context string (e.g., "[socket]tests/test.py::test_name")

    Returns:
        Tuple of (command, args) for pexpect.spawn
    """
    full_args = [mpremote_path] + mpremote_args

    # Check if coverage is active
    if os.environ.get("COVERAGE_PROCESS_START") and context:
        # Wrap with coverage run --context=...
        # This is the same pattern used by run-mpremote-tests.sh
        return "coverage", ["run", f"--context={context}"] + full_args
    else:
        # Run directly with Python
        return sys.executable, full_args


def is_wsl():
    """Detect if running under Windows Subsystem for Linux (WSL)."""
    try:
        with open("/proc/version", "r") as f:
            return "microsoft" in f.read().lower()
    except (FileNotFoundError, PermissionError):
        return False


def get_devices():
    """
    Get list of devices to test against from MPREMOTE_DEVICES or MPREMOTE_DEVICE env var.

    Returns list of (device_url, scenario_name) tuples.
    """
    devices_str = os.environ.get("MPREMOTE_DEVICES", "")
    if not devices_str:
        # Fall back to singular MPREMOTE_DEVICE
        device_str = os.environ.get("MPREMOTE_DEVICE", "")
        if device_str:
            devices_str = device_str

    if not devices_str:
        return []

    devices = [d.strip() for d in devices_str.split(",") if d.strip()]
    return [(device, get_scenario_from_device(device)) for device in devices]
