#!/usr/bin/env python3

import re
import subprocess
import sys

verbosity = 0  # Show what's going on, 0 1 or 2.
suggestions = 1  # Set to 0 to not include lengthy suggestions in error messages.

ignore_prefixes = []


def verbose(*args):
    if verbosity:
        print(*args)


def very_verbose(*args):
    if verbosity > 1:
        print(*args)


def git_log(pretty_format, *args):
    # Delete pretty argument from user args so it doesn't interfere with what we do.
    args = ["git", "log"] + [arg for arg in args if "--pretty" not in args]
    args.append("--pretty=format:" + pretty_format)
    very_verbose("git_log", *args)
    # Generator yielding each output line.
    for line in subprocess.Popen(args, stdout=subprocess.PIPE).stdout:
        yield line.decode().rstrip("\r\n")


def verify(sha):
    verbose("verify", sha)
    errors = []
    warnings = []

    def error_text(err):
        return "commit " + sha + ": " + err

    def error(err):
        errors.append(error_text(err))

    def warning(err):
        warnings.append(error_text(err))

    # Author and committer email.
    for line in git_log("%ae%n%ce", sha, "-n1"):
        very_verbose("email", line)
        if "noreply" in line:
            error("Unwanted email address: " + line)

    # Message body.
    raw_body = list(git_log("%B", sha, "-n1"))
    verify_message_body(raw_body, error, warning)
    return errors, warnings


def verify_message_body(raw_body, error, warning):
    if not raw_body:
        error("Message is empty")
        return

    # Subject line.
    subject_line = raw_body[0]
    for prefix in ignore_prefixes:
        if subject_line.startswith(prefix):
            verbose("Skipping ignored commit message")
            return
    very_verbose("subject_line", subject_line)
    subject_line_format = r"^[^!]+: [A-Z]+.+ .+\.$"
    if not re.match(subject_line_format, subject_line):
        error("Subject line should match " + repr(subject_line_format) + ": " + subject_line)
    if len(subject_line) >= 73:
        error("Subject line should be 72 or less characters: " + subject_line)

    # Second one divides subject and body.
    if len(raw_body) > 1 and raw_body[1]:
        error("Second message line should be empty: " + raw_body[1])

    # Message body lines.
    for line in raw_body[2:]:
        # Long lines with URLs are exempt from the line length rule.
        if len(line) >= 76 and "://" not in line:
            error("Message lines should be 75 or less characters: " + line)

    if not raw_body[-1].startswith("Signed-off-by: ") or "@" not in raw_body[-1]:
        warning("Message should be signed-off")


def run(args):
    verbose("run", *args)
    has_errors = False
    has_warnings = False

    if "--check-file" in args:
        has_errors, has_warnings = run_check_file(args[-1])
    else:  # Normal operation
        for sha in git_log("%h", *args):
            errors, warnings = verify(sha)
            has_errors |= any(errors)
            has_warnings |= any(warnings)
            for err in errors:
                print("error:", err)
            for err in warnings:
                print("warning:", err)
    if has_errors or has_warnings:
        if suggestions:
            print("See https://github.com/micropython/micropython/blob/master/CODECONVENTIONS.md")
    else:
        print("ok")
    if has_errors:
        sys.exit(1)


def run_check_file(filename):
    verbose("checking commit message from", filename)
    errors = []
    warnings = []

    def error(err):
        errors.append(err)

    def warning(err):
        warnings.append(err)

    with open(args[-1]) as f:
        lines = [l.rstrip("\r\n") for l in f]
        verify_message_body(lines, error, warning)

    for err in errors:
        print("error:", err)
    for err in warnings:
        print("warning:", err)

    return any(errors), any(warnings)


def show_help():
    print("usage: verifygitlog.py [-v -n -h --check-file] ...")
    print("-v  : increase verbosity, can be speficied multiple times")
    print("-n  : do not print multi-line suggestions")
    print("-h  : print this help message and exit")
    print(
        "--check-file : Pass a single argument which is a file containing a candidate commit message"
    )
    print(
        "--ignore-rebase : Skip checking commits with git rebase autosquash prefixes or WIP as a prefix"
    )
    print("... : arguments passed to git log to retrieve commits to verify")
    print("      see https://www.git-scm.com/docs/git-log")
    print("      passing no arguments at all will verify all commits")
    print("examples:")
    print("verifygitlog.py -n10  # Check last 10 commits")
    print("verifygitlog.py -v master..HEAD  # Check commits since master")


if __name__ == "__main__":
    args = sys.argv[1:]
    verbosity = args.count("-v")
    suggestions = args.count("-n") == 0
    if "--ignore-rebase" in args:
        args.remove("--ignore-rebase")
        ignore_prefixes = ["squash!", "fixup!", "amend!", "WIP"]

    if "-h" in args:
        show_help()
    else:
        args = [arg for arg in args if arg not in ["-v", "-n", "-h"]]
        run(args)
