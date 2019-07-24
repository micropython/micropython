#!/usr/bin/python

import os.path
import re
import subprocess
import sys
import tempfile

def main(): # pragma: no cover
    files = sys.argv[1:]
    failed_files = commit_is_ready(files)
    if failed_files:
        print("\nYou may run the following command to repeat the check:")
        print("Hint: You may need to be at the repository's root directory.")
        print("\n" + " \\\n".join([sys.argv[0]] + failed_files) + "\n")
        print('Aborting Commit.')
        sys.exit(1)
    sys.exit(0)


def commit_is_ready(file_names=""):
    """Return False if not ready.  Return True if commit is ready"""
    if not file_names:
        file_names = get_modified_files()

    files_to_check = [f for f in file_names if file_exists(f) and file_is_checkable(f)]

    checks = [
        check_secrets,
        check_hungarian_notation,
        check_uncrustify,
        check_whitespace,
    ]

    # If there is no file to check, return. This can happen if all the modified
    # files are ignored as defined in is_ignored_file_pattern.
    if files_to_check:
        for check in checks:
            failed_files = check(files_to_check)
            if failed_files:
                return failed_files
    return []


def get_modified_files():
    changed_files = subprocess.check_output(
        "git diff-index --name-only --cached HEAD", shell=True)
    if type(changed_files) is not str:
        changed_files = changed_files.decode('utf-8')
    file_names = changed_files.split('\n')
    return file_names


def file_is_checkable(file_name):
    return (
        is_source_file(file_name)
        and is_checked_file_pattern(file_name)
        and not is_ignored_file_pattern(file_name)
    )


def file_exists(file_name):
    return os.path.isfile(file_name)


def is_source_file(file_name):
    if re.findall(r'\.[ch]$', file_name):
        return True
    return False


def is_checked_file_pattern(file_name):
    checked_patterns = [
        r"demos/common/",
        r"lib/.*aws_",
        r"lib/.*\.c",
        r"tests/common/",
        r"tools/",
    ]
    for checked_pattern in checked_patterns:
        if re.findall(checked_pattern, file_name):
            return True
    return False


def is_ignored_file_pattern(file_name):
    ignored_patterns = [
        "lib/FreeRTOS-Plus-POSIX/",
        "lib/FreeRTOS-Plus-TCP/",
        "lib/FreeRTOS/",
        "lib/include/FreeRTOS_POSIX/",
        "lib/include/private/threading_alt.h",
        "lib/ota/portable",
        "lib/pkcs11/",
        "lib/secure_sockets/portable",
        "lib/third_party/",
        "lib/wifi/portable",
        "tests/common/posix",
    ]
    for ignored_pattern in ignored_patterns:
        if re.findall(ignored_pattern, file_name):
            return True
    return False


def check_secrets(changed_files):
    if not changed_files:
        return []
    if subprocess.call("git secrets --scan " + " ".join(changed_files), shell=True):
        return ['git_secrets']
    return []


def check_whitespace(changed_files):
    """Return True if check failed.  Return False if check passed"""
    if subprocess.call("git diff-index --check --cached HEAD", shell=True):
        return ['whitespace']
    return []


def check_hungarian_notation(changed_files):
    """Return True if check failed.  Return False if check passed"""
    failed_files = []
    for changed_file in changed_files:
        if subprocess.call(
            (
                "python tools/checks/style/hn_check/src/hn_check.py " +
                changed_file
            ), shell=True
        ):
            failed_files.append(changed_file)
    return failed_files


def check_uncrustify(changed_files):
    """Return True if check failed.  Return False if check passed"""
    if "" == changed_files:
        return False
    failed_files = []
    for changed_file in changed_files:
        if subprocess.call(
            (
                "uncrustify --check -q -c .uncrustify.cfg " +
                changed_file
            ), shell=True
        ):
            failed_files.append(changed_file)
    if failed_files:
        patch = patch_uncrustify(changed_files)
        write_patch(patch)
    return failed_files

def patch_uncrustify(changed_files):
    """Creates patch to fix formatting in a set of files"""
    patch = ''
    for file in changed_files:
        format_call = (
            'uncrustify -q -c .uncrustify.cfg -f {}'.format(file)
            + '| git --no-pager diff --color=always --no-index -- "{}" - '.format(file)
            + '| tail -n+3'
        )
        diff_result = subprocess.check_output(format_call, shell=True)
        if type(diff_result) is not str:
            diff_result = diff_result.decode("utf-8")

        diff_result = re.sub(r'---.*', '--- "a/{}"'.format(file), diff_result)
        diff_result = re.sub(r'\+\+\+.*', '+++ "b/{}"'.format(file), diff_result)
        patch += diff_result
    return patch


def write_patch(patch): # pragma: no cover
    """Writes patch to temporary file and prints instructions to stdout"""
    ansi_escape = re.compile(r'\x1B\[[0-?]*[ -/]*[@-~]')
    colorless_patch = ansi_escape.sub('', patch)
    tmp_dir = tempfile.mkdtemp()
    patch_file_name = os.path.join(tmp_dir, 'uncrustify_patch')
    with open(patch_file_name, 'w') as patch_file:
        patch_file.write(colorless_patch)
    print("The staged files are not formatted correctly.")
    # Todo: Default to print colored patch
    # Use command line flag to print colorless
    print(colorless_patch)
    print("you may apply the patch using the following command:")
    print("")
    print("        git apply {}".format(patch_file_name))
    print("")


if __name__ == "__main__":  # pragma: no cover
    main()
