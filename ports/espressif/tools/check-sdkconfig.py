#!/usr/bin/env python3
import sys

import click


def int_or_string(s):
    try:
        return int(s)
    except ValueError:
        return s.strip('"')


def collect_definitions(file):
    """Collect all definitions in supplied sdkconfig.h."""
    sdk_config = {}
    for line in file:
        if line.startswith("#define "):
            _, k, v = line.strip().split(None, 2)
            # Handle transitive definitions like '#define CONFIG_TCP_MSL CONFIG_LWIP_TCP_MSL'
            v = sdk_config.get(k, v)
            sdk_config[k] = int_or_string(v)
    return sdk_config


def validate(sdk_config, circuitpy_config):
    partition_table = sdk_config.get("CONFIG_PARTITION_TABLE_FILENAME")
    for var in ("CIRCUITPY_STORAGE_EXTEND", "CIRCUITPY_DUALBANK"):
        if circuitpy_config.get(var):
            with open(partition_table) as f:
                content = f.read()
                if not "ota_1" in content:
                    raise SystemExit(
                        f"{var} is incompatible with {partition_table=} (no ota_1 partition)"
                    )

    # Add more checks here for other things we want to verify.
    return


@click.command()
@click.argument("definitions", nargs=-1, metavar="CIRCUITPY_X=1 CIRCUITPY_Y=0 ...")
@click.argument(
    "sdkconfig_h", required=True, nargs=1, type=click.File("r"), metavar="<path to sdkconfig.h>"
)
def run(definitions, sdkconfig_h):
    sdk_config = collect_definitions(sdkconfig_h)

    # Parse definitions arguments
    circuitpy_config = {}
    for definition in definitions:
        k, v = definition.split("=", 1)
        circuitpy_config[k] = int_or_string(v)

    # Validate.
    validate(sdk_config, circuitpy_config)


if __name__ == "__main__":
    run()
