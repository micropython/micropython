#!/usr/bin/env python3
import sys

sdk_config = {}

config_h = sys.argv[-1]
with open(config_h) as f:
    for row in f:
        if row.startswith("#define "):
            _, k, v = row.strip().split(None, 2)
            # ad-hoc handle lines like '#define CONFIG_TCP_MSL CONFIG_LWIP_TCP_MSL'
            v = sdk_config.get(k, v)
            if v[0] == '"':
                v = eval(v)  # Assume it is a simple C string

            # ad-hoc convert to integer
            try:
                v = int(v)
            except ValueError:
                pass
            sdk_config[k] = v

del sys.argv[-1]

circuitpy_config = {}
for arg in sys.argv[1:]:
    k, v = arg.split("=", 1)
    circuitpy_config[k] = int(v)

partition_table = sdk_config.get("CONFIG_PARTITION_TABLE_FILENAME")
for var in ("CIRCUITPY_STORAGE_EXTEND", "CIRCUITPY_DUALBANK"):
    if circuitpy_config.get(var):
        with open(partition_table) as f:
            content = f.read()
        if not "ota_1" in content:
            raise SystemExit(f"{var} is incompatible with {partition_table=} (no ota_1 partition)")

    # Add more checks here
