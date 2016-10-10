#
# @file harness.py
#
# @section License
# Copyright (C) 2016, Erik Moqvist
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# This file is part of the Pumbaa project.
#

import os
import io
import sys

class TestCaseSkippedError(Exception):
    pass


class SuiteError(Exception):

    def __init__(self, total, passed, skipped, failed):
        super().__init__(self)
        self.total = total
        self.passed = passed
        self.skipped = skipped
        self.failed = failed


class ExceptionNotRaisedError(Exception):
    pass


class AssertRaises(object):

    def __init__(self, expected_type, expected_message):
        self.expected_type = expected_type
        self.expected_message = expected_message

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, tb):
        if exc_type is None:
            raise ExceptionNotRaisedError("{} not raised".format(
                self.expected_type))
        elif exc_type == self.expected_type:
            if self.expected_message in [None, str(exc_value)]:
                return True


def assert_raises(expected_type, expected_message=None):
    return AssertRaises(expected_type, expected_message)


def run(testcases):
    """Run all test cases in the list.

    """

    # Print a header.
    print()
    print("================================== TEST BEGIN ==================================\n")
    print()

    passed = 0
    skipped = 0
    failed = 0
    total = len(testcases)

    for callback, name in testcases:
        print("enter:", name)

        try:
            callback()
            passed += 1
            print("exit: {}: PASSED\n".format(name))
        except TestCaseSkippedError:
            skipped += 1
            print("exit: {}: SKIPPED\n".format(name))
        except Exception as e:
            failed += 1
            sys.print_exception(e)
            print("exit: {}: FAILED\n".format(name))

    ok = passed + skipped == total

    print("harness report: total({}), passed({}), failed({}), skipped({})\n".format(
        total, passed, failed, skipped))

    print("=============================== TEST END ({}) ==============================\n".format(
        "PASSED" if ok else "FAILED"))

    print(os.system("kernel/thrd/list"))
    
    if not ok:
        raise SuiteError(total, passed, skipped, failed)
