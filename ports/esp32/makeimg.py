# Combine bootloader, partition table and application into a final binary.

import os, sys

sys.path.append(os.getenv("IDF_PATH") + "/components/partition_table")

import gen_esp32part

OFFSET_BOOTLOADER = 0x1000
OFFSET_PARTITIONS = 0x8000


def load_partition_table(filename):
    with open(filename, "rb") as f:
        return gen_esp32part.PartitionTable.from_binary(f.read())


partition_table = load_partition_table(sys.argv[2])

max_size_bootloader = OFFSET_PARTITIONS - OFFSET_BOOTLOADER
max_size_partitions = 0
offset_application = 0
max_size_application = 0

for part in partition_table:
    if part.name == "nvs":
        max_size_partitions = part.offset - OFFSET_PARTITIONS
    elif part.type == gen_esp32part.APP_TYPE and offset_application == 0:
        offset_application = part.offset
        max_size_application = part.size

files_in = [
    ("bootloader", OFFSET_BOOTLOADER, max_size_bootloader, sys.argv[1]),
    ("partitions", OFFSET_PARTITIONS, max_size_partitions, sys.argv[2]),
    ("application", offset_application, max_size_application, sys.argv[3]),
]
file_out = sys.argv[4]

cur_offset = OFFSET_BOOTLOADER
with open(file_out, "wb") as fout:
    for name, offset, max_size, file_in in files_in:
        assert offset >= cur_offset
        fout.write(b"\xff" * (offset - cur_offset))
        cur_offset = offset
        with open(file_in, "rb") as fin:
            data = fin.read()
            fout.write(data)
            cur_offset += len(data)
            print(
                "%-12s@0x%06x % 8d  (% 8d remaining)"
                % (name, offset, len(data), max_size - len(data))
            )
            if len(data) > max_size:
                print(
                    "ERROR: %s overflows allocated space of %d bytes by %d bytes"
                    % (name, max_size, len(data) - max_size)
                )
                sys.exit(1)
    print("%-22s% 8d" % ("total", cur_offset))
