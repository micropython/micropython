try:
    from importlib.metadata import version, PackageNotFoundError

    try:
        __version__ = version("mpremote")
    except PackageNotFoundError:
        # Error loading package version (e.g. running from source).
        __version__ = "0.0.0-local"
except ImportError:
    # importlib.metadata not available (e.g. CPython <3.8 without
    # importlib_metadata compatibility package installed).
    __version__ = "0.0.0-unknown"
