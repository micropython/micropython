#!/usr/bin/env python3
#
# Usage:
#
# $ verifygitlog.py
# $ git log --format=full -n 10 | verifygitlog.py
#

import io, re, select, subprocess, sys

RED = "\033[0;31m"
NC = "\033[0m"  # No Color


def process_commit(commit_hash, author, committer, msg):
    all_ok = True

    def log_warn(msg):
        nonlocal all_ok
        all_ok = False
        print(RED, commit_hash, " ", msg, NC, sep="")

    for name, email in (author, committer):
        warn = False
        if email.find("noreply") != -1:
            warn = True
        if warn:
            log_warn(f"Bad email: {name} <{email}>")

    assert len(msg) == 1 or len(msg) >= 3

    if len(msg[0]) >= 73:
        log_warn(f"Heading too long: {msg[0]}")

    if msg[0].find(": ") == -1:
        log_warn(f"No ':' in heading")

    if msg[0].find(".c: ") != -1:
        log_warn(f"Source '.c' extension in heading")

    if len(msg) > 1:
        assert len(msg[1]) == 0

        for m in msg[2:]:
            if len(m) >= 76:
                log_warn(f"Body too long: {m}")

    if not msg[-1].startswith("Signed-off-by: "):
        log_warn("Not signed off")

    if all_ok:
        print(commit_hash, "OK", msg[0])


def get_line(r=None):
    l = input_source.readline()
    if r is None:
        return l
    m = re.match(r, l)
    assert m, l
    return m.groups()


def main():
    # Detect stdin, or use default git-log command.
    global input_source
    rlist, _, _ = select.select([sys.stdin], [], [], 0)
    if rlist:
        input_source = sys.stdin
    else:
        proc = subprocess.run(["git", "log", "--format=full", "-n21"], capture_output=True)
        input_source = io.StringIO(str(proc.stdout, "utf8"))

    while True:
        commit_hash = get_line(r"commit ([a-z0-9]{40})$")
        author = get_line(r"Author: ([\w ]+) <([A-Za-z0-9+.]+@[A-Za-z0-9.-]+)>$")
        committer = get_line(r"Commit: ([\w ]+) <([A-Za-z0-9+.]+@[A-Za-z0-9.-]+)>$")
        # date = get_line(r"Date:   ([A-Za-z0-9:+ ]+)$")
        get_line(r"^$")

        last_line = False
        msg = []
        while True:
            line = get_line()
            if not line:
                last_line = True
                break
            if line.startswith("    "):
                msg.append(line.strip())
            else:
                assert line == "\n"
                break

        process_commit(commit_hash[0], author, committer, msg)

        if last_line:
            break


main()
