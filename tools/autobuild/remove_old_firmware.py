#!/usr/bin/env python3

import re, subprocess, sys


DEBUG = False
DRY_RUN = False
NUM_KEEP_PER_BOARD = 4


def main():
    ssh_machine = sys.argv[1]
    ssh_firmware_dir = sys.argv[2]

    # SSH to get list of existing files.
    p = subprocess.run(
        ["ssh", ssh_machine, "find", ssh_firmware_dir, "-name", "\\*-unstable-v\\*"],
        capture_output=True,
    )
    if p.returncode != 0:
        print(p.stderr)
        return
    all_files = p.stdout.split(b"\n")

    # Parse all files to organise into boards/date/version.
    boards = {}
    for file in all_files:
        m = re.match(
            rb"([a-z/.]+)/([A-Za-z0-9_-]+)-(20[0-9]{6})-unstable-(v[0-9.-]+-g[0-9a-f]+).",
            file,
        )
        if not m:
            continue
        dir, board, date, version = m.groups()
        if board not in boards:
            boards[board] = {}
        if (date, version) not in boards[board]:
            boards[board][(date, version)] = []
        boards[board][(date, version)].append(file)

    # Collect files to remove based on date and version.
    remove = []
    for board in boards.values():
        filelist = [(date, version, files) for (date, version), files in board.items()]
        filelist.sort(reverse=True)
        keep = []
        for date, version, files in filelist:
            if keep and version == keep[-1]:
                remove.extend(files)
            elif len(keep) >= NUM_KEEP_PER_BOARD:
                remove.extend(files)
            else:
                keep.append(version)

    if DEBUG:
        all_files.sort(reverse=True)
        for file in all_files:
            print(file, file in remove)
        print(len(remove), "/", len(all_files))

    # Do removal of files.
    for file in remove:
        file = str(file, "ascii")
        print("remove:", file)
        if not DRY_RUN:
            p = subprocess.run(["ssh", ssh_machine, "/bin/rm", file], capture_output=True)
            if p.returncode != 0:
                print(p.stderr)


if __name__ == "__main__":
    main()
