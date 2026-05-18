#!/usr/bin/env python3
#
# This file is part of the MicroPython project, https://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Alessandro Gatti
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import enum
import functools
import json
import pathlib
import sys
from urllib import parse as urllib_parse


class ViolationKind(enum.Enum):
    SUGGESTION = enum.auto()
    WARNING = enum.auto()
    ERROR = enum.auto()
    NONE = enum.auto()


MANDATORY_KEYS = ("deploy", "docs", "url", "features", "images", "mcu", "product", "vendor")
OPTIONAL_KEYS = ("deploy_options", "features_non_filterable", "thumbnail", "variants")


KNOWN_IMAGE_EXTENSIONS = (".jpg", ".png", ".webp", ".gif")

# TODO: Should all of these be kept or leave just a subset?
# TODO: Should `microSD` and `SDCard` be merged into one entry?
KNOWN_FEATURES = (
    "10BASE-T1S",
    "12x12 RGB LED Matrix",
    "3 Axis IMU",
    "5x5 RGB LED Matrix",
    "Audio Codec",
    "BLE",
    "Battery Charging",
    "CAN",
    "Camera",
    "Capacitive Touch",
    "DAC",
    "Display",
    "Dual-core",
    "Environment Sensor",
    "Ethernet",
    "External Flash",
    "External RAM",
    "Feather",
    "I2C BAT Fuel Gauge",
    "I2S Audio Amplifier + Speaker",
    "IMU",
    "JST-PH",
    "JST-SH",
    "LoRa",
    "Microphone",
    "PoE",
    "RGB LED",
    "SDCard",
    "Second LDO",
    "Secure Element",
    "T1S",
    "TinyPICO Compatible",
    "TinyPICO Nano Compatible",
    "USB",
    "USB-C",
    "Watch",
    "WiFi",
    "microSD",
    "mikroBUS",
)


def tag_lists(tag, *lists):
    tagged = []
    for old_list in lists:
        tagged.append([(tag, entry) for entry in old_list])
    return tagged


def check_deploy(board_json, board_file):
    errors = []
    warnings = []

    deploy = board_json.get("deploy", None)
    if deploy is None:
        errors.append("missing key")
    else:
        # TODO: Can be a plain string too?
        if type(deploy) is list:
            for entry in deploy:
                if type(entry) is str:
                    entry_path = board_file.parent / entry
                    if not entry_path.exists():
                        errors.append('missing file: "{}"'.format(entry))
                else:
                    errors.append("non-string element: {}".repr(entry))
        else:
            errors.append("must be a list")
    return tag_lists("deploy", [], errors, warnings)


def generic_check_url(key, board_json, *_, empty=None):
    errors = []
    warnings = []

    value = board_json.get(key, None)
    if value is None:
        errors.append("missing key")
    else:
        if type(value) is str:
            if value.strip() != value:
                warnings.append("leading/trailing whitespace")
            if value:
                if parsed_url := urllib_parse.urlsplit(value.strip()):
                    scheme, *_ = parsed_url
                    if scheme not in ("http", "https", "ftp"):
                        errors.append("unknown URL scheme {}".format(scheme))
                else:
                    errors.append("potential malformed URL {}".format(value))
            else:
                if empty is ViolationKind.WARNING:
                    warnings.append("value is empty")
                elif empty is ViolationKind.ERROR:
                    errors.append("value is empty")
                elif empty is ViolationKind.NONE:
                    pass
                else:
                    raise Exception("unexpected violation kind value: {}".format(repr(empty)))
        else:
            errors.append("must be a string")
    return tag_lists(key, [], errors, warnings)


def generic_check_features(key, board_json, *_, missing=None):
    suggestions = []
    errors = []
    warnings = []

    features = board_json.get(key, None)
    if features is None:
        if missing is ViolationKind.ERROR:
            errors.append("missing key")
        elif missing is ViolationKind.WARNING:
            errors.append("missing key")
        elif missing is ViolationKind.NONE:
            pass
        else:
            raise Exception("unexpected violation kind value: {}".format(repr(missing)))
    else:
        if type(features) is list:
            if not features:
                suggestions.append("list is empty")
            else:
                for feature in features:
                    if type(feature) is str:
                        if feature != feature.strip():
                            warnings.append("`{}` has trailing/leading whitespace".format(feature))
                        feature = feature.strip()
                        if feature not in KNOWN_FEATURES:
                            warnings.append(
                                "`{}` is not known (possible misspelling?)".format(feature)
                            )
                    else:
                        errors.append("non-string element: {}".repr(feature))
        else:
            errors.append("must be a list")

    return tag_lists(key, suggestions, errors, warnings)


def _check_image_path(image, empty):
    error = None
    warnings = []
    if type(image) is str:
        if image != image.strip():
            warnings.append("`{}` has trailing/leading whitespace".format(image))
        if not image:
            if empty is ViolationKind.ERROR:
                error = "value is empty"
            elif empty is ViolationKind.WARNING:
                warnings.append("value is empty")
            elif empty is ViolationKind.NONE:
                pass
            else:
                raise Exception("unexpected violation kind value: {}".format(repr(empty)))
        else:
            image_path = pathlib.PosixPath(image)
            if image_path.suffix not in KNOWN_IMAGE_EXTENSIONS:
                if image_path.suffix.lower() in KNOWN_IMAGE_EXTENSIONS:
                    warnings.append('non-lowercase extension: "{}"'.format(image_path))
                else:
                    error = 'unknown extension: "{}"'.format(image_path)
    else:
        error = "non-string element: {}".repr(image)

    return error, warnings


def check_images(board_json, *_):
    errors = []
    warnings = []

    images = board_json.get("images", None)
    if images is None:
        errors.append("missing key")
    else:
        if type(images) is list:
            if not images:
                warnings.append("list is empty")
            else:
                for image in images:
                    new_error, new_warnings = _check_image_path(image, ViolationKind.ERROR)
                    if new_error:
                        errors.append(new_error)
                    elif new_warnings:
                        warnings.extend(new_warnings)
        else:
            errors.append("must be a list")

    return tag_lists("images", [], errors, warnings)


def check_thumbnail(board_json, *_):
    suggestions = []
    errors = []
    warnings = []

    thumbnail = board_json.get("thumbnail", None)
    if thumbnail is None:
        suggestions.append("missing key")
    else:
        new_error, new_warnings = _check_image_path(thumbnail, ViolationKind.WARNING)
        if new_error:
            errors.append(new_error)
        elif new_warnings:
            warnings.extend(new_warnings)

    return tag_lists("thumbnail", suggestions, errors, warnings)


def generic_check_dict(key, board_json, *_, missing=None):
    errors = []
    warnings = []

    dictionary = board_json.get(key, None)
    if dictionary is None:
        if missing is ViolationKind.ERROR:
            errors.append("missing key")
        elif missing is ViolationKind.WARNING:
            errors.append("missing key")
        elif missing is ViolationKind.NONE:
            pass
        else:
            raise Exception("unexpected violation kind value: {}".format(repr(missing)))
    else:
        if type(dictionary) is dict:
            if not dictionary:
                warnings.append("dict is empty")
            else:
                for key, value in dictionary.items():
                    if type(key) is not str:
                        errors.append("non-string key: {}".format(repr(key)))
                    if type(value) is not str:
                        errors.append(
                            "non-string value: {} for key {}".format(repr(value), repr(key))
                        )
                    if not key:
                        errors.append("empty key found")
                    if key != key.strip():
                        errors.append("key {} has leading/trailing whitespace".format(key))
                    if not value:
                        errors.append("empty value found for key {}".format(key))
                    if value != value.strip():
                        warnings.append(
                            "value {} for key {} has leading/trailing whitespace".format(
                                value, key
                            )
                        )
        else:
            errors.append("must be a dict")

    return tag_lists(key, [], errors, warnings)


def generic_check_string(key, board_json, *_):
    errors = []
    warnings = []
    value = board_json.get(key, None)
    if value is None:
        errors.append("missing key")
    else:
        if type(value) is str:
            if value.strip() != value:
                warnings.append("leading/trailing whitespace")
            if not value:
                errors.append("value is empty")
        else:
            errors.append("must be a string")
    return tag_lists(key, [], errors, warnings)


if len(sys.argv) < 2:
    print("Usage: {} <board JSON metadata file>")
    sys.exit()


board_file = pathlib.PosixPath(sys.argv[1])
if not board_file.exists():
    raise FileNotFoundError('board path "{}" was not found'.format(sys.argv[1]))
elif not board_file.is_file():
    raise IOError('board path "{}" does not point to a file'.format(sys.argv[1]))

print("Checking {}...".format(sys.argv[1]))

suggestions = []
errors = []
warnings = []

with board_file.open("rt") as board_json_file:
    board_json = json.load(board_json_file)
    if type(board_json) is dict:
        for checker in (
            check_deploy,
            functools.partial(generic_check_url, "docs", empty=ViolationKind.WARNING),
            functools.partial(generic_check_url, "url", empty=ViolationKind.ERROR),
            functools.partial(generic_check_features, "features", missing=ViolationKind.ERROR),
            functools.partial(
                generic_check_features, "features_non_filterable", missing=ViolationKind.NONE
            ),
            check_images,
            functools.partial(generic_check_string, "mcu"),
            functools.partial(generic_check_string, "product"),
            functools.partial(generic_check_string, "vendor"),
            check_thumbnail,
            functools.partial(generic_check_dict, "deploy_options", missing=ViolationKind.NONE),
            functools.partial(generic_check_dict, "variants", missing=ViolationKind.NONE),
        ):
            new_suggestions, new_errors, new_warnings = checker(board_json, board_file)
            suggestions.extend(new_suggestions)
            errors.extend(new_errors)
            warnings.extend(new_warnings)

        for key in board_json:
            if key not in MANDATORY_KEYS and key not in OPTIONAL_KEYS:
                errors.append(("<root>", "unknown key {}".format(repr(key))))
    else:
        errors.append(("<root>", "JSON must only contain a single object"))

print(
    "\n{} error(s), {} warning(s), and {} suggestion(s) found:".format(
        len(errors), len(warnings), len(suggestions)
    )
)

if errors:
    print("\nERRORS:")
    for key, error in errors:
        print("× {} - {}".format(key, error))

if warnings:
    print("\nWARNINGS:")
    for key, warning in warnings:
        print("⚠ {} - {}".format(key, warning))

if suggestions:
    print("\nSUGGESTIONS:")
    for key, suggestion in suggestions:
        print("! {} - {}".format(key, suggestion))

print()
