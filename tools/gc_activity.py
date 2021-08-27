# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import sys
import json

# Map start block to current allocation info.
current_heap = {}
allocation_history = []
root = {}


def change_root(trace, size):
    level = root
    for frame in reversed(trace):
        file_location = frame[1]
        if file_location not in level:
            level[file_location] = {
                "blocks": 0,
                "file": file_location,
                "function": frame[2],
                "subcalls": {},
            }
        level[file_location]["blocks"] += size
        level = level[file_location]["subcalls"]


total_actions = 0
with open(sys.argv[1], "r") as f:
    for line in f:
        if not line.strip():
            break
    for line in f:
        action = None
        if line.startswith("Breakpoint 2"):
            break
        next(f)  # throw away breakpoint code line
        next(f)  # first frame
        block = 0
        size = 0
        trace = []
        for line in f:
            # print(line.strip())
            if line[0] == "#":
                frame = line.strip().split()
                if frame[1].startswith("0x"):
                    trace.append((frame[1], frame[-1], frame[3]))
                else:
                    trace.append(("0x0", frame[-1], frame[1]))
            elif line[0] == "$":
                block = int(line.strip().split()[-1][2:], 16)
                size = int(next(f).strip().split()[-1][2:], 16)
            if not line.strip():
                break

        action = "unknown"
        if block not in current_heap:
            current_heap[block] = {
                "start_block": block,
                "size": size,
                "start_trace": trace,
                "start_time": total_actions,
            }
            action = "alloc"
            change_root(trace, size)
        else:
            alloc = current_heap[block]
            alloc["end_trace"] = trace
            alloc["end_time"] = total_actions
            change_root(alloc["start_trace"], -1 * alloc["size"])
            if size > 0:
                action = "realloc"
                current_heap[block] = {
                    "start_block": block,
                    "size": size,
                    "start_trace": trace,
                    "start_time": total_actions,
                }
                change_root(trace, size)
            else:
                action = "free"
                if trace[0][2] == "gc_sweep":
                    action = "sweep"
                del current_heap[block]
            alloc["end_cause"] = action
            allocation_history.append(alloc)
        print(total_actions, action, block, size)
        total_actions += 1

print()

for alloc in current_heap.values():
    alloc["end_trace"] = ""
    alloc["end_time"] = total_actions
    allocation_history.append(alloc)


def print_frame(frame, indent=0):
    for key in sorted(frame):
        if (
            not frame[key]["blocks"]
            or key.startswith("../py/malloc.c")
            or key.startswith("../py/gc.c")
        ):
            continue
        print(" " * (indent - 1), key, frame[key]["function"], frame[key]["blocks"], "blocks")
        print_frame(frame[key]["subcalls"], indent + 2)


print_frame(root)
total_blocks = 0
for key in sorted(root):
    total_blocks += root[key]["blocks"]
print(total_blocks, "total blocks")

with open("allocation_history.json", "w") as f:
    json.dump(allocation_history, f)
