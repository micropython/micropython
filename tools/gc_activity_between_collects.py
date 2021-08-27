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
non_single_block_streak = 0
max_nsbs = 0
last_action = None
last_total_actions = 0
count = 0
actions = {}
last_ticks_ms = 0
ticks_ms = 0
block_sizes = {}
allocation_sources = {}
with open(sys.argv[1], "r") as f:
    for line in f:
        if not line.strip():
            break
    for line in f:
        action = None
        if line.startswith("Breakpoint 2"):
            break
        next(f)  # throw away breakpoint code line
        # print(next(f)) # first frame
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
                # print(line.strip().split()[-1])
                block = int(line.strip().split()[-1][2:], 16)
                next_line = next(f)
                size = int(next_line.strip().split()[-1][2:], 16)
                # next_line = next(f)
                # ticks_ms = int(next_line.strip().split()[-1][2:], 16)
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
            if size == 1:
                max_nsbs = max(max_nsbs, non_single_block_streak)
                non_single_block_streak = 0
            else:
                non_single_block_streak += 1
            # change_root(trace, size)
            if size not in block_sizes:
                block_sizes[size] = 0
            source = trace[-1][-1]
            if source not in allocation_sources:
                print(trace)
                allocation_sources[source] = 0
            allocation_sources[source] += 1
            block_sizes[size] += 1
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
                # change_root(trace, size)
            else:
                action = "free"
                if trace[0][2] == "gc_sweep":
                    action = "sweep"
                    non_single_block_streak = 0
                if (
                    trace[3][2] == "py_gc_collect" or (trace[3][2] == "gc_deinit" and count > 1)
                ) and last_action != "sweep":
                    print(
                        ticks_ms - last_ticks_ms,
                        total_actions - last_total_actions,
                        "gc.collect",
                        max_nsbs,
                    )
                    print(actions)
                    print(block_sizes)
                    print(allocation_sources)
                    actions = {}
                    block_sizes = {}
                    allocation_sources = {}
                    if count % 2 == 0:
                        print()
                    count += 1
                    last_total_actions = total_actions
                    last_ticks_ms = ticks_ms
                    max_nsbs = 0
                del current_heap[block]
            alloc["end_cause"] = action
            allocation_history.append(alloc)
        if action not in actions:
            actions[action] = 0
        actions[action] += 1
        last_action = action
        # print(total_actions, non_single_block_streak, action, block, size)
        total_actions += 1
print(actions)
print(max_nsbs)
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


# print_frame(root)
# total_blocks = 0
# for key in sorted(root):
#     total_blocks += root[key]["blocks"]
# print(total_blocks, "total blocks")

# with open("allocation_history.json", "w") as f:
#     json.dump(allocation_history, f)
