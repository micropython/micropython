#!/usr/bin/env python3
"""Run tests under one or more MicroPython variants and emit a markdown
comparison report.

Usage (run from anywhere):
    tools/make_report.py --variant name=/path/to/micropython [--variant ...] \
                         [--keep-path name] [--keep-path ...] \
                         [--out report.md] [paths ...]

`paths` may be individual `.py` files or directories (recursed for `.py`),
resolved relative to the repository's `tests/` directory.
Defaults to `typing typing/pep`.

Each variant gets a column. Cell values use short tags:
    ok  xfail  false_positive  skip  FAIL  ERROR
    skip covers both runtime `skipTest()` calls and testcases whose whole
    file SKIP'd (or crashed); the file's tests are still
    listed if another variant ran them.

`--keep-path` can be specified for individual variants by name. If specified
for a variant, MICROPYPATH will not be overridden when running tests for
that variant, preserving the existing environment's module search path.

By default, paths are split in auto mode: unittest-style tests are run directly
and parsed from unittest output, while traditional tests under `basics` are run
through tests/run-tests.py and parsed from tests/results/.../_results.json.
Use --mode to force one style.

Use --hide-ok/--hide-skip/--hide-xfail to hide rows where all variants are in the
selected status set.
"""

import argparse
import datetime
import json
import os
import re
import subprocess
import sys
from collections import OrderedDict

HERE = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.abspath(os.path.join(HERE, ".."))
TESTS_DIR = os.path.join(REPO_ROOT, "tests")

# unittest line:  "test_name (qualified.Class) ... <result>"
_LINE_RE = re.compile(r"^(?P<name>\S+) \((?P<cls>[^)]+)\) \.\.\.\s*(?P<result>.+?)\s*$")
_ANSI_ESCAPE_RE = re.compile(r"\x1B\[[0-9;]*m")


def get_git_info(repo_path):
    """Return (branch_or_tag, commit_hash) for a given repository path.
    Returns (None, None) if git info cannot be retrieved."""
    try:
        ref_name = "unknown"

        # Try to get branch name first
        proc = subprocess.run(
            ["git", "symbolic-ref", "--short", "HEAD"],
            cwd=repo_path,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=5,
        )
        if proc.returncode == 0:
            ref_name = proc.stdout.decode("utf-8").strip()
        else:
            # Not on a branch (detached HEAD), try to get a tag
            proc = subprocess.run(
                ["git", "describe", "--tags", "--exact-match"],
                cwd=repo_path,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=5,
            )
            if proc.returncode == 0:
                ref_name = proc.stdout.decode("utf-8").strip()
            else:
                # No exact tag, try git describe for a meaningful ref
                proc = subprocess.run(
                    ["git", "describe", "--all", "--always"],
                    cwd=repo_path,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    timeout=5,
                )
                if proc.returncode == 0:
                    ref_name = proc.stdout.decode("utf-8").strip()

        # Get commit hash
        proc = subprocess.run(
            ["git", "rev-parse", "HEAD"],
            cwd=repo_path,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=5,
        )
        commit = proc.stdout.decode("utf-8").strip()[:12] if proc.returncode == 0 else "unknown"

        return ref_name, commit
    except (FileNotFoundError, subprocess.TimeoutExpired):
        return None, None


_RESULT_TAG = {
    "ok": "ok",
    "FAIL": "FAIL",
    "ERROR": "ERROR",
    "expected failure": "xfail",
    "unexpected success": "false_positive",
}

# Additional tag mappings for newer exception types, if available.
# These map exception type names to short tags for the new unittest version.
_EXCEPTION_TYPE_TAG = {
    "ExpectedFailure": "xfail",
    "UnexpectedPass": "false_positive",
}

_TRADITIONAL_STATUS_TAG = {
    "pass": "ok",
    "skip": "skip",
    "fail": "FAIL",
    "ignored": "xfail",
}


def parse_unittest_output(text):
    """Return list of (cls, name, tag) parsed from unittest stdout.

    Backward compatible with both old and new MicroPython unittest versions:
    - Old: parses string tags like "expected failure", "unexpected success"
    - New: also handles ExpectedFailure and UnexpectedPass exception types in output
    """
    results = []
    for line in text.splitlines():
        clean_line = strip_ansi(line)
        m = _LINE_RE.match(clean_line)
        if not m:
            continue
        raw = strip_ansi(m.group("result")).strip()
        if raw.startswith("skipped"):
            tag = "skip"
        elif "ExpectedFailure" in raw:
            tag = _EXCEPTION_TYPE_TAG.get("ExpectedFailure", "xfail")
        elif "UnexpectedPass" in raw:
            tag = _EXCEPTION_TYPE_TAG.get("UnexpectedPass", "false_positive")
        else:
            tag = _RESULT_TAG.get(raw, raw)
        results.append((m.group("cls"), m.group("name"), tag))
    return results


def strip_ansi(text):
    return _ANSI_ESCAPE_RE.sub("", text)


def read_header_comments(file_path, max_lines=5):
    """Return up to max_lines of leading `#` comment lines from file_path,
    with the leading '#' and one optional space stripped. Stops at the first
    non-comment, non-blank line. Blank lines within the leading block are
    skipped."""
    lines = []
    try:
        with open(file_path, "r", encoding="utf-8") as fh:
            for raw in fh:
                s = raw.rstrip("\n")
                stripped = s.lstrip()
                if not stripped:
                    if lines:
                        break
                    continue
                if not stripped.startswith("#"):
                    break
                text = stripped[1:]
                if text.startswith(" "):
                    text = text[1:]
                lines.append(text)
                if len(lines) >= max_lines:
                    break
    except OSError:
        return []
    return lines


def collect_test_files(paths):
    files = []
    seen = set()

    def add_test_file(path):
        rel = os.path.relpath(path, TESTS_DIR)
        if rel not in seen:
            seen.add(rel)
            files.append(rel)

    for p in paths:
        full = p if os.path.isabs(p) else os.path.join(TESTS_DIR, p)
        if os.path.isfile(full) and full.endswith(".py"):
            add_test_file(full)
        elif os.path.isdir(full):
            for root, dirnames, filenames in os.walk(full):
                dirnames.sort()
                for entry in sorted(filenames):
                    if not entry.endswith(".py"):
                        continue
                    add_test_file(os.path.join(root, entry))
        else:
            print("warning: skipping %s (not found)" % p, file=sys.stderr)
    return files


def normalize_input_path(path):
    """Return path relative to tests/ if possible, preserving order-friendly '/'."""
    if os.path.isabs(path):
        try:
            rel = os.path.relpath(path, TESTS_DIR)
            if not rel.startswith(".." + os.sep) and rel != "..":
                path = rel
        except ValueError:
            pass
    return path.replace("\\", "/")


def _to_rel_test_path(path):
    """Return test path relative to tests/ and normalized to '/'."""
    return os.path.relpath(path, TESTS_DIR).replace("\\", "/")


def split_paths_by_mode(paths, mode):
    unittest_paths = []
    traditional_paths = []

    for p in paths:
        rel = normalize_input_path(p).lstrip("./")
        if mode == "unittest":
            unittest_paths.append(p)
            continue
        if mode == "traditional":
            traditional_paths.append(p)
            continue

        # auto mode
        root = rel.split("/", 1)[0]
        is_traditional = root == "basics"
        if is_traditional:
            traditional_paths.append(p)
        else:
            unittest_paths.append(p)

    return unittest_paths, traditional_paths


def is_whole_file_skip(text, max_lines=5):
    for line in text.splitlines()[:max_lines]:
        if strip_ansi(line).lstrip().startswith("SKIP"):
            return True
    return False


def run_variant(binary, test_file, keep_path=False):
    """Run one test file under one variant. Return (stdout, returncode)."""
    env = os.environ.copy()
    if not keep_path:
        env["MICROPYPATH"] = os.pathsep.join(
            (
                ".frozen",
                os.path.join(REPO_ROOT, "extmod"),
                os.path.join(REPO_ROOT, "lib", "micropython-lib", "python-stdlib", "unittest"),
            )
        )
    try:
        proc = subprocess.run(
            [binary, test_file],
            cwd=TESTS_DIR,
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            timeout=120,
        )
        return proc.stdout.decode("utf-8", "replace"), proc.returncode
    except subprocess.TimeoutExpired:
        return "TIMEOUT", -1
    except FileNotFoundError:
        return "BINARY NOT FOUND: %s" % binary, -1


def run_traditional_variant(binary, files, result_dir, keep_path=False):
    """Run traditional tests via run-tests.py.

    Return (stdout_text, returncode, status_map) where status_map maps
    'dir/name.py' -> short result tag.
    """
    env = os.environ.copy()
    env["MICROPY_MICROPYTHON"] = binary
    cmd = [sys.executable, "run-tests.py", "-r", result_dir, "-j", "1"]
    if keep_path:
        cmd.append("--keep-path")
    cmd.extend(normalize_input_path(p) for p in files)

    try:
        proc = subprocess.run(
            cmd,
            cwd=TESTS_DIR,
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            timeout=1800,
        )
        text = proc.stdout.decode("utf-8", "replace")
    except subprocess.TimeoutExpired:
        return "TIMEOUT", -1, {}
    except FileNotFoundError:
        return "PYTHON NOT FOUND: %s" % sys.executable, -1, {}

    status_map = {}
    results_file = os.path.join(result_dir, "_results.json")
    try:
        with open(results_file, "r", encoding="utf-8") as fh:
            payload = json.load(fh)
        for item in payload.get("results", []):
            if not isinstance(item, list) and not isinstance(item, tuple):
                continue
            if len(item) < 2:
                continue
            test_file, status = item[0], item[1]
            tag = _TRADITIONAL_STATUS_TAG.get(status)
            if tag is None:
                continue
            key = str(test_file).replace("\\", "/")
            status_map[key] = tag
    except (OSError, json.JSONDecodeError):
        # Return empty map; caller decides whether to mark tests as skip or ERROR.
        pass

    return text, proc.returncode, status_map


def sanitize_name(name):
    return re.sub(r"[^A-Za-z0-9_.-]", "_", name)


def get_binary_size(binary):
    """Return dict with text/data/bss/total sizes (bytes) for `binary`,
    or None if `size` is unavailable or the binary doesn't exist."""
    if not os.path.isfile(binary):
        return None
    try:
        proc = subprocess.run(
            ["size", binary],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=10,
        )
    except (FileNotFoundError, subprocess.TimeoutExpired):
        return None
    if proc.returncode != 0:
        return None
    lines = proc.stdout.decode("utf-8", "replace").splitlines()
    if len(lines) < 2:
        return None
    # Expected: "   text\tdata\tbss\tdec\thex\tfilename"
    parts = lines[1].split()
    if len(parts) < 4:
        return None
    try:
        text, data, bss, total = (int(parts[i]) for i in range(4))
    except ValueError:
        return None
    return {"text": text, "data": data, "bss": bss, "total": total}


def _fmt_kb(n):
    if n is None:
        return "-"
    return "{:,}".format(n).replace(",", "_")


def normalize_status_tag(tag):
    """Return a plain status token for row filtering."""
    return str(tag).replace("**", "").strip()


def should_hide_row(cells, hide_statuses):
    """Return True when every cell status belongs to hide_statuses."""
    return bool(hide_statuses) and all(normalize_status_tag(c) in hide_statuses for c in cells)


def render_markdown(
    variant_names,
    unittest_files,
    traditional_files,
    table,
    file_summary,
    sizes,
    headers,
    metadata,
    hide_statuses=None,
):
    hide_statuses = set(hide_statuses or ())
    hidden_rows = 0
    out = []
    out.append("# Test report\n")
    out.append("**Summary:**\n")

    # Add metadata section
    if metadata:
        cmdline_paths = metadata.get("cmdline_paths") or []
        hide_opts = metadata.get("hide_options") or []
        out.append(
            " - Specified test(s): "
            + (", ".join(cmdline_paths) if cmdline_paths else "(default)")
            + "\n"
        )
        out.append(f"   - micropython: {metadata['main_branch']} ({metadata['main_commit']})\n")
        if metadata.get("lib_branch") and metadata.get("lib_commit"):
            out.append(
                f"   - micropython-lib: {metadata['lib_branch']} ({metadata['lib_commit']})\n"
            )
        out.append(f" - Generated: {metadata['timestamp']}\n")
        out.append(" - Hide options: " + (", ".join(hide_opts) if hide_opts else "none") + "\n")
        out.append(f" - Rows hidden: {hidden_rows}\n")
        out.append("")

    out.append(
        "| Variant | PASS | xfail | skip | FAIL | False Positive | ERROR | total | text | data | bss | size | Δsize |"
    )
    out.append("|:---|" + "---:|" * 12)
    baseline_size = None
    if variant_names:
        first_size = (sizes.get(variant_names[0]) or {}).get("total")
        baseline_size = first_size
    for v in variant_names:
        c = file_summary[v]
        s = sizes.get(v) or {}
        total_size = s.get("total")
        if total_size is None or baseline_size is None:
            delta = "-"
        elif v == variant_names[0]:
            delta = "ref"
        else:
            d = total_size - baseline_size
            delta = ("{:+,}".format(d)).replace(",", "_")
        out.append(
            "| %s | %d | %d | %d | %d | %d | %d | %d | %s | %s | %s | %s | %s |"
            % (
                v,
                c["ok"],
                c["xfail"],
                c["skip"],
                c["FAIL"],
                c["false_positive"],
                c["ERROR"],
                c["total"],
                _fmt_kb(s.get("text")),
                _fmt_kb(s.get("data")),
                _fmt_kb(s.get("bss")),
                _fmt_kb(s.get("total")),
                delta,
            )
        )
    out.append("")

    out.append(
        "\n"
        "Legend:\n"
        " - **ok**=passed, **xfail**=expected failure, **skip**=test skipped\n"
        " - **FAIL**=test failed, **False Positive**=unexpected success\n"
        " - **ERROR**=test errored (also used when the test module crashed).\n"
        " - Firmware sizes are reported in bytes from `size <binary>`.\n"
    )

    for f in unittest_files:
        rows = [(k, v) for k, v in table.items() if k[0] == f]
        if not rows:
            continue
        rendered_rows = []
        for (_f, cls, name), per in rows:
            cells = [per.get(v, "skip") for v in variant_names]
            if should_hide_row(cells, hide_statuses):
                hidden_rows += 1
                continue
            rendered_rows.append((cls, name, cells))
        if not rendered_rows:
            continue
        out.append("## " + f)
        hdr_lines = headers.get(f) or []
        if hdr_lines:
            for hl in hdr_lines:
                out.append("    " + hl)
            out.append("")
        header = "| Class | Test | " + " | ".join(variant_names) + " |"
        sep = "|" + "---|" * (2 + len(variant_names))
        out.append(header)
        out.append(sep)
        for cls, name, cells in rendered_rows:
            out.append("| %s | %s | %s |" % (cls, name, " | ".join(cells)))
        out.append("")

    grouped = {}
    for f in traditional_files:
        folder = os.path.dirname(f).replace("\\", "/")
        if not folder:
            folder = "."
        grouped.setdefault(folder, []).append(f)

    for folder in sorted(grouped):
        files = sorted(grouped[folder])
        if folder in (".", "./"):
            folder_label = "tests"
        else:
            folder_label = f"tests/{folder}"
        rendered_rows = []
        for f in files:
            key = (f, "traditional", "result")
            per = table.get(key)
            if not per:
                continue
            cells = [per.get(v, "skip") for v in variant_names]
            if should_hide_row(cells, hide_statuses):
                hidden_rows += 1
                continue
            rendered_rows.append((os.path.basename(f), cells))
        if not rendered_rows:
            continue
        out.append("## Folder: " + folder_label)
        header = "| Test | " + " | ".join(variant_names) + " |"
        sep = "|" + "---|" * (1 + len(variant_names))
        out.append(header)
        out.append(sep)
        for test_name, cells in rendered_rows:
            out.append("| %s | %s |" % (test_name, " | ".join(cells)))
        out.append("")
    if metadata:
        out[6] = f" - Rows hidden: {hidden_rows}\n"

    return "\n".join(out)


def bold_failures(text):
    return re.sub(r"\b(FAIL|ERROR|false_positive)\b", r"**\1**", text)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument(
        "--variant",
        action="append",
        required=True,
        help="name=path to a micropython binary (may be repeated)",
    )
    ap.add_argument(
        "--keep-path",
        action="append",
        default=[],
        help="variant name for which to keep MICROPYPATH (do not override it)",
    )
    ap.add_argument(
        "--mode",
        choices=("auto", "unittest", "traditional"),
        default="auto",
        help="test processing mode (default: auto)",
    )
    ap.add_argument("--hide-ok", action="store_true", help="hide rows where all variants are ok")
    ap.add_argument(
        "--hide-skip", action="store_true", help="hide rows where all variants are skip"
    )
    ap.add_argument(
        "--hide-xfail", action="store_true", help="hide rows where all variants are xfail"
    )
    ap.add_argument("--out", default="-", help="markdown output file (default: stdout)")
    ap.add_argument(
        "paths",
        nargs="*",
        default=["typing", "typing/pep"],
        help="test files or directories (relative to tests/)",
    )
    args = ap.parse_args()

    variants = OrderedDict()
    for spec in args.variant:
        if "=" not in spec:
            ap.error("--variant must be name=path")
        name, _, path = spec.partition("=")
        if not os.path.isabs(path):
            path = os.path.abspath(os.path.join(REPO_ROOT, path))
        variants[name] = path

    # Build a set of variant names for which to keep the path
    keep_path_variants = set(args.keep_path)

    unittest_paths, traditional_paths = split_paths_by_mode(args.paths, args.mode)

    if args.mode == "auto":
        # Always include both suites in auto mode.
        for default_path in ("typing_runtime", "typing", "typing/pep"):
            full = os.path.join(TESTS_DIR, default_path)
            if os.path.isdir(full) or (os.path.isfile(full) and full.endswith(".py")):
                unittest_paths.append(default_path)
        basics_dir = os.path.join(TESTS_DIR, "basics")
        if os.path.isdir(basics_dir):
            traditional_paths.append("basics")

    unittest_paths = list(
        OrderedDict((normalize_input_path(p), None) for p in unittest_paths).keys()
    )
    traditional_paths = list(
        OrderedDict((normalize_input_path(p), None) for p in traditional_paths).keys()
    )

    unittest_files = collect_test_files(unittest_paths)
    traditional_files = collect_test_files(traditional_paths)

    table = OrderedDict()  # (file, cls, name) -> {variant: tag}
    file_status = {}  # (file, variant) -> "ran" | "skip" | "crash"
    file_summary = {
        v: dict.fromkeys(("ok", "xfail", "false_positive", "skip", "FAIL", "ERROR", "total"), 0)
        for v in variants
    }
    sizes = {v: get_binary_size(p) for v, p in variants.items()}

    for f in unittest_files:
        for vname, vbin in variants.items():
            keep_path = vname in keep_path_variants
            text, rc = run_variant(vbin, f, keep_path=keep_path)
            if is_whole_file_skip(text):
                file_status[(f, vname)] = "skip"
                continue
            if rc != 0:
                file_status[(f, vname)] = "crash"
            else:
                file_status[(f, vname)] = "ran"
            cases = parse_unittest_output(text)
            for cls, name, tag in cases:
                if cls.startswith("__main__."):
                    cls = cls.replace("__main__.", "")
                key = (f, cls, name)
                if key not in table:
                    table[key] = {}
                # Count using the original tag before bolding
                if tag in file_summary[vname]:
                    file_summary[vname][tag] += 1
                file_summary[vname]["total"] += 1
                # Apply bold formatting for display
                tag = bold_failures(tag)
                table[key][vname] = tag

    # Second pass: testcases that some variants ran but others did not are
    # counted as `skip` (whole-file SKIP) or `ERROR` (variant crashed) so
    # per-variant totals match the global row count.
    for key, per in table.items():
        f = key[0]
        for vname in variants:
            if vname in per:
                continue
            status = file_status.get((f, vname), "skip")
            tag = "ERROR" if status == "crash" else "skip"
            per[vname] = bold_failures(tag)
            file_summary[vname][tag] += 1
            file_summary[vname]["total"] += 1

    for vname, vbin in variants.items():
        if not traditional_paths:
            break
        keep_path = vname in keep_path_variants
        result_dir = os.path.join(TESTS_DIR, "results", "make_report_" + sanitize_name(vname))
        os.makedirs(result_dir, exist_ok=True)
        _text, rc, status_map = run_traditional_variant(
            vbin,
            traditional_files,
            result_dir=result_dir,
            keep_path=keep_path,
        )

        for f in traditional_files:
            tag = status_map.get(f)
            if tag is None:
                tag = "ERROR" if rc != 0 else "skip"
            key = (f, "traditional", "result")
            if key not in table:
                table[key] = {}
            table[key][vname] = bold_failures(tag)
            if tag in file_summary[vname]:
                file_summary[vname][tag] += 1
            file_summary[vname]["total"] += 1

    headers = {f: read_header_comments(os.path.join(TESTS_DIR, f)) for f in unittest_files}

    # Gather metadata
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    main_branch, main_commit = get_git_info(REPO_ROOT)
    lib_path = os.path.join(REPO_ROOT, "lib", "micropython-lib")
    lib_branch, lib_commit = get_git_info(lib_path) if os.path.isdir(lib_path) else (None, None)

    metadata = {
        "timestamp": timestamp,
        "main_branch": main_branch or "unknown",
        "main_commit": main_commit or "unknown",
        "lib_branch": lib_branch,
        "lib_commit": lib_commit,
        "cmdline_paths": args.paths,
        "hide_options": [],
    }

    hide_statuses = set()
    if args.hide_ok:
        hide_statuses.add("ok")
        metadata["hide_options"].append("--hide-ok")
    if args.hide_skip:
        hide_statuses.add("skip")
        metadata["hide_options"].append("--hide-skip")
    if args.hide_xfail:
        hide_statuses.add("xfail")
        metadata["hide_options"].append("--hide-xfail")

    md = render_markdown(
        list(variants),
        unittest_files,
        traditional_files,
        table,
        file_summary,
        sizes,
        headers,
        metadata,
        hide_statuses=hide_statuses,
    )
    if args.out == "-":
        print(md)
    else:
        with open(args.out, "w") as fh:
            fh.write(md)
        print("Wrote %s (%d rows)" % (args.out, len(table)), file=sys.stderr)


if __name__ == "__main__":
    main()
