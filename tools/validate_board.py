#!/usr/bin/env python3
"""Validate new MicroPython board definitions in a pull request.

Checks file presence, board.json schema conformance, and content quality
for boards that are new in the current PR (not modifications to existing boards).

Usage:
    python tools/validate_board.py --base-ref <sha> [--schema tools/board_schema.json] \
        [--media-json /tmp/media_warnings.json] <changed-file> ...
"""

import argparse
import json
import os
import subprocess
import sys
import urllib.error
import urllib.request

try:
    import jsonschema
except ImportError:
    print("Error: jsonschema package is required. Install with: uv pip install jsonschema")
    sys.exit(1)


# Ports that use CMake; all others use Make.
CMAKE_PORTS = {"esp32", "rp2"}

MEDIA_BASE_URL = (
    "https://raw.githubusercontent.com/micropython/micropython-media/master/boards"
)


class ValidationResult:
    """Accumulates errors and warnings for board validation."""

    def __init__(self):
        self.errors = []
        self.warnings = []
        self.notices = []

    def error(self, file_path, message):
        self.errors.append((file_path, message))

    def warning(self, file_path, message):
        self.warnings.append((file_path, message))

    def notice(self, file_path, message):
        self.notices.append((file_path, message))

    def emit_github_annotations(self):
        for file_path, message in self.errors:
            print(f"::error file={file_path}::{message}")
        for file_path, message in self.warnings:
            print(f"::warning file={file_path}::{message}")
        for file_path, message in self.notices:
            print(f"::notice file={file_path}::{message}")

    def format_markdown(self, boards):
        """Format validation results as markdown."""
        lines = ["## Board Validation Results\n"]
        if not boards:
            lines.append("No new boards detected in this PR.\n")
        else:
            for port, board_name, _ in boards:
                board_errors = [
                    (f, m)
                    for f, m in self.errors
                    if f"boards/{board_name}/" in f or f.endswith(board_name)
                ]
                board_warnings = [
                    (f, m)
                    for f, m in self.warnings
                    if f"boards/{board_name}/" in f or f.endswith(board_name)
                ]
                board_notices = [
                    (f, m)
                    for f, m in self.notices
                    if f"boards/{board_name}/" in f or f.endswith(board_name)
                ]
                status = ":x:" if board_errors or board_warnings else ":white_check_mark:"
                lines.append(
                    f"### {status} {board_name} ({port})"
                    f" — {len(board_errors)} error(s),"
                    f" {len(board_warnings)} warning(s),"
                    f" {len(board_notices)} notice(s)\n"
                )
                if board_errors or board_warnings or board_notices:
                    lines.append("| Status | File | Details |")
                    lines.append("|--------|------|---------|")
                    for f, m in board_errors:
                        lines.append(f"| :x: Error | `{os.path.basename(f)}` | {m} |")
                    for f, m in board_warnings:
                        lines.append(f"| :warning: Warning | `{os.path.basename(f)}` | {m} |")
                    for f, m in board_notices:
                        lines.append(
                            f"| :information_source: Notice | `{os.path.basename(f)}` | {m} |"
                        )
                    lines.append("")
        return "\n".join(lines)

    def write_step_summary(self, boards):
        summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
        if not summary_path:
            return
        with open(summary_path, "a") as f:
            f.write(self.format_markdown(boards))


def collect_existing_values(repo_root):
    """Scan all existing board.json files to collect known MCU and vendor values."""
    import glob

    mcus = set()
    vendors = set()
    for board_json_path in glob.glob(
        os.path.join(repo_root, "ports", "*", "boards", "*", "board.json")
    ):
        try:
            with open(board_json_path) as f:
                data = json.load(f)
            if "mcu" in data:
                mcus.add(data["mcu"])
            if "vendor" in data:
                vendors.add(data["vendor"])
        except (json.JSONDecodeError, OSError):
            pass
    return {"mcus": mcus, "vendors": vendors}


def detect_new_boards(changed_files, repo_root, base_ref):
    """Identify boards that are new in this PR (not modifications to existing boards).

    A board is considered "new" if its board.json did not exist in the base ref.
    """
    boards = {}
    for filepath in changed_files:
        # Match ports/{port}/boards/{BOARD_NAME}/...
        parts = filepath.split("/")
        if len(parts) >= 5 and parts[0] == "ports" and parts[2] == "boards":
            port = parts[1]
            board_name = parts[3]
            board_dir = os.path.join(repo_root, "ports", port, "boards", board_name)
            boards[(port, board_name)] = board_dir

    new_boards = []
    for (port, board_name), board_dir in sorted(boards.items()):
        git_path = f"ports/{port}/boards/{board_name}/board.json"
        result = subprocess.run(
            ["git", "cat-file", "-e", f"{base_ref}:{git_path}"],
            capture_output=True,
            cwd=repo_root,
        )
        if result.returncode != 0:
            # board.json did NOT exist in base -- this is a new board
            new_boards.append((port, board_name, board_dir))

    return new_boards


def check_file_presence(board_dir, port, board_name, result):
    """Check that required board files are present."""
    board_json_path = os.path.join(board_dir, "board.json")
    rel_dir = os.path.relpath(board_dir)

    # board.json
    if not os.path.isfile(board_json_path):
        result.error(rel_dir, "Missing required file: board.json")
        return  # Can't do further checks without board.json

    # mpconfigboard.h
    if not os.path.isfile(os.path.join(board_dir, "mpconfigboard.h")):
        result.error(
            os.path.join(rel_dir, "mpconfigboard.h"),
            "Missing required file: mpconfigboard.h",
        )

    # Build system config
    if port in CMAKE_PORTS:
        expected = "mpconfigboard.cmake"
    else:
        expected = "mpconfigboard.mk"
    if not os.path.isfile(os.path.join(board_dir, expected)):
        result.error(
            os.path.join(rel_dir, expected),
            f"Missing required file: {expected} (expected for {port} port)",
        )

    # pins.csv -- warning only, except for GENERIC boards
    if "GENERIC" not in board_name:
        if not os.path.isfile(os.path.join(board_dir, "pins.csv")):
            result.warning(
                os.path.join(rel_dir, "pins.csv"),
                "Missing pins.csv. This file is expected for non-generic boards.",
            )


def check_board_json_schema(board_json_path, schema, result):
    """Validate board.json against the JSON Schema."""
    rel_path = os.path.relpath(board_json_path)
    try:
        with open(board_json_path) as f:
            data = json.load(f)
    except json.JSONDecodeError as e:
        result.error(rel_path, f"Invalid JSON: {e}")
        return None
    except OSError as e:
        result.error(rel_path, f"Cannot read file: {e}")
        return None

    validator = jsonschema.Draft7Validator(schema)
    for error in sorted(validator.iter_errors(data), key=lambda e: list(e.path)):
        path = ".".join(str(p) for p in error.absolute_path) or "(root)"
        result.error(rel_path, f"Schema validation error at {path}: {error.message}")

    return data


def check_board_json_content(board_json_path, data, existing_values, result):
    """Run semantic checks on board.json content."""
    rel_path = os.path.relpath(board_json_path)

    # Unknown MCU
    mcu = data.get("mcu", "")
    if mcu and mcu not in existing_values["mcus"]:
        result.warning(
            rel_path,
            f"Unknown MCU '{mcu}'. If this is a new MCU, this may be expected. "
            f"Known values: {', '.join(sorted(existing_values['mcus']))}",
        )

    # Unknown vendor
    vendor = data.get("vendor", "")
    if vendor and vendor not in existing_values["vendors"]:
        result.warning(
            rel_path,
            f"Unknown vendor '{vendor}'. If this is a new vendor, this may be expected. "
            f"Known values: {', '.join(sorted(existing_values['vendors']))}",
        )

    # Vendor name duplicated in product name
    product = data.get("product", "")
    if vendor and product and vendor.lower() in product.lower():
        result.warning(
            rel_path,
            f"Vendor name '{vendor}' appears in product name '{product}'. "
            "The vendor is displayed separately, so it should not be repeated in the product name.",
        )

    # Empty fields that should have content
    if data.get("url") == "":
        result.warning(
            rel_path,
            "url is empty. A product or vendor URL should be provided.",
        )
    if data.get("docs") == "":
        result.warning(rel_path, "docs is empty. Consider adding a docs URL.")


def check_url_liveness(board_json_path, data, result):
    """Check that the board URL is reachable. Warnings only."""
    rel_path = os.path.relpath(board_json_path)
    url = data.get("url", "")
    if not url:
        return

    try:
        req = urllib.request.Request(url, method="HEAD")
        req.add_header("User-Agent", "MicroPython-Board-Validator/1.0")
        with urllib.request.urlopen(req, timeout=10) as resp:
            if resp.status >= 400:
                result.notice(
                    rel_path,
                    f"URL '{url}' returned HTTP {resp.status}.",
                )
    except urllib.error.HTTPError as e:
        # Some servers reject HEAD requests, try GET
        if e.code == 405:
            try:
                req = urllib.request.Request(url, method="GET")
                req.add_header("User-Agent", "MicroPython-Board-Validator/1.0")
                with urllib.request.urlopen(req, timeout=10) as resp:
                    if resp.status >= 400:
                        result.notice(
                            rel_path,
                            f"URL '{url}' returned HTTP {resp.status}.",
                        )
            except Exception as e2:
                result.notice(rel_path, f"URL '{url}' is not reachable: {e2}")
        else:
            result.notice(rel_path, f"URL '{url}' returned HTTP {e.code}.")
    except Exception as e:
        result.notice(rel_path, f"URL '{url}' is not reachable: {e}")


def check_media_images(board_json_path, board_name, data, result, media_warnings):
    """Check if board images exist in the micropython-media repository."""
    rel_path = os.path.relpath(board_json_path)
    images = data.get("images", [])
    missing_images = []

    for image in images:
        url = f"{MEDIA_BASE_URL}/{board_name}/{image}"
        try:
            req = urllib.request.Request(url, method="HEAD")
            req.add_header("User-Agent", "MicroPython-Board-Validator/1.0")
            with urllib.request.urlopen(req, timeout=10) as resp:
                if resp.status >= 400:
                    missing_images.append(image)
        except Exception:
            missing_images.append(image)

    if missing_images:
        images_str = ", ".join(missing_images)
        result.notice(
            rel_path,
            f"Image(s) not found in micropython-media repository: {images_str}. "
            "Please submit a corresponding PR to "
            "https://github.com/micropython/micropython-media with board images.",
        )
        media_warnings.append(
            {
                "board_json": rel_path,
                "missing_images": missing_images,
            }
        )


def main():
    parser = argparse.ArgumentParser(description="Validate new MicroPython board definitions.")
    parser.add_argument(
        "--repo-root",
        default=".",
        help="Root of the MicroPython repository (default: current directory).",
    )
    parser.add_argument(
        "--base-ref",
        required=True,
        help="Base branch SHA to compare against for detecting new boards.",
    )
    parser.add_argument(
        "--schema",
        default=None,
        help="Path to board_schema.json (default: tools/board_schema.json in repo root).",
    )
    parser.add_argument(
        "--media-json",
        default=None,
        help="Path to write media warnings JSON for PR commenting.",
    )
    parser.add_argument(
        "--results-json",
        default=None,
        help="Path to write full validation results JSON for PR commenting.",
    )
    parser.add_argument(
        "--comment-md",
        default=None,
        help="Path to write formatted markdown for PR comment.",
    )
    parser.add_argument(
        "changed_files",
        nargs="*",
        help="List of files changed in the PR.",
    )

    args = parser.parse_args()
    repo_root = os.path.abspath(args.repo_root)

    schema_path = args.schema or os.path.join(repo_root, "tools", "board_schema.json")
    try:
        with open(schema_path) as f:
            schema = json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        print(f"::error::Cannot load schema from {schema_path}: {e}")
        sys.exit(1)

    # Detect new boards
    new_boards = detect_new_boards(args.changed_files, repo_root, args.base_ref)
    if not new_boards:
        print("No new boards detected in this PR.")
        result = ValidationResult()
        result.write_step_summary([])
        sys.exit(0)

    board_names = ", ".join(f"{b[1]} ({b[0]})" for b in new_boards)
    print(f"Validating {len(new_boards)} new board(s): {board_names}")

    # Collect existing values for MCU/vendor checks
    existing_values = collect_existing_values(repo_root)

    result = ValidationResult()
    media_warnings = []

    for port, board_name, board_dir in new_boards:
        print(f"\n--- Checking {board_name} ({port}) ---")
        board_json_path = os.path.join(board_dir, "board.json")

        # 1. File presence
        check_file_presence(board_dir, port, board_name, result)

        # 2. Schema validation (only if board.json exists)
        if os.path.isfile(board_json_path):
            data = check_board_json_schema(board_json_path, schema, result)

            if data is not None:
                # 3. Content checks
                check_board_json_content(board_json_path, data, existing_values, result)

                # 4. URL liveness
                check_url_liveness(board_json_path, data, result)

                # 5. Media image check
                check_media_images(board_json_path, board_name, data, result, media_warnings)

    # Output
    result.emit_github_annotations()
    result.write_step_summary(new_boards)

    # Write media warnings JSON for PR comment step
    if args.media_json and media_warnings:
        with open(args.media_json, "w") as f:
            json.dump(media_warnings, f, indent=2)

    # Write full validation results JSON for PR comment step
    if args.results_json:
        results_data = {
            "boards": [
                {"port": port, "board_name": name} for port, name, _ in new_boards
            ],
            "errors": [{"file": f, "message": m} for f, m in result.errors],
            "warnings": [{"file": f, "message": m} for f, m in result.warnings],
            "notices": [{"file": f, "message": m} for f, m in result.notices],
        }
        with open(args.results_json, "w") as f:
            json.dump(results_data, f, indent=2)

    # Write formatted markdown for step summary
    if args.comment_md and (result.errors or result.warnings or result.notices):
        with open(args.comment_md, "w") as f:
            f.write(result.format_markdown(new_boards))

    # Summary
    print(f"\n{'='*60}")
    print(
        f"Errors: {len(result.errors)}, Warnings: {len(result.warnings)}, "
        f"Notices: {len(result.notices)}"
    )
    if result.errors:
        print("\nErrors (must fix):")
        for f, m in result.errors:
            print(f"  {f}: {m}")
    if result.warnings:
        print("\nWarnings (should fix):")
        for f, m in result.warnings:
            print(f"  {f}: {m}")
    if result.notices:
        print("\nNotices (informational):")
        for f, m in result.notices:
            print(f"  {f}: {m}")

    if result.errors:
        sys.exit(1)
    elif result.warnings:
        sys.exit(2)
    else:
        sys.exit(0)


if __name__ == "__main__":
    main()
