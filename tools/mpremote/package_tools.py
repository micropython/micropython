import os
import shutil
import mpremote
from pathlib import Path
from subprocess import run
from setuptools.build_meta import (
    get_requires_for_build_wheel,
    get_requires_for_build_sdist,
    prepare_metadata_for_build_wheel,
    build_wheel as real_build_wheel,
    build_sdist as real_build_sdist,
)


MPREMOTE = Path(__file__).parent
PACKAGE = MPREMOTE / "mpremote"
TOOLS_DIR = MPREMOTE / "../../tools"
VERSION_FILE = PACKAGE / "__version__.py"


def set_version_number(vers):
    mpremote.__version__ = vers
    VERSION_FILE.write_text(f'__version__ = "{vers}"\n')


def check_development_version():
    """
    Checks the number of commits since mpremote version was changed. In non-zero this
    is appended to the version number to denote it as a development release.
    """
    try:
        last_commit = (
            run(
                ["git", "log", "--follow", "-1", "--pretty=%H", "mpremote/__version__.py"],
                cwd=MPREMOTE,
                capture_output=True,
            )
            .stdout.strip()
            .decode()
        )
        if not last_commit:
            # Not running from git repo - likely sdist. Use version as-is
            return
        num_commits = (
            run(
                ["git", "rev-list", "--count", f"{last_commit}..HEAD"],
                cwd=MPREMOTE,
                capture_output=True,
            )
            .stdout.strip()
            .decode()
        )
        num_commits = int(num_commits)
        if num_commits:
            original_version = mpremote.__version__
            base_version = mpremote.__version__.split("+")[0]
            set_version_number(f"{base_version}+{num_commits}")
            return original_version
    except (FileNotFoundError, TypeError) as ex:
        # Not running from git repo - likely sdist. Use version as-is.
        return


def copy_tools():
    """
    Include copies of out-of-tree tools in package.
    """
    tools = ("pyboard.py",)
    if TOOLS_DIR.exists():
        for t in tools:
            f = TOOLS_DIR / t
            print("copy {} -> {}".format(f, PACKAGE))
            shutil.copy(f, PACKAGE)
    else:
        if any((not (PACKAGE / t).exists() for t in tools)):
            raise SystemExit("Cannot find tools to include")


def build_wheel(wheel_directory, config_settings=None, metadata_directory=None):
    original_version = check_development_version()
    copy_tools()
    ret = real_build_wheel(wheel_directory, config_settings, metadata_directory)
    if original_version:
        set_version_number(original_version)
    return ret


def build_sdist(sdist_directory, config_settings=None):
    original_version = check_development_version()
    copy_tools()
    ret = real_build_sdist(sdist_directory, config_settings)
    if original_version:
        set_version_number(original_version)
    return ret
