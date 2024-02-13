"""This script updates the sdkconfigs based on the menuconfig results in a given
   build."""

import pathlib
import click
import copy
import kconfiglib
import os

OPT_SETTINGS = [
    "CONFIG_ESP_ERR_TO_NAME_LOOKUP",
    "CONFIG_ESP_CONSOLE_",
    "CONFIG_CONSOLE_UART_",
    "CONFIG_ESP_SYSTEM_PANIC_",
    "CONFIG_ESP32S2_PANIC_",
    "COMPILER_OPTIMIZATION_",
    "CONFIG_ESP32S3_DEBUG_OCDAWARE",
    "CONFIG_FREERTOS_ASSERT_",
    "CONFIG_FREERTOS_DEBUG_OCDAWARE",
    "CONFIG_FREERTOS_TASK_FUNCTION_WRAPPER",
    "CONFIG_HAL_ASSERTION_",
    "CONFIG_LWIP_ESP_LWIP_ASSERT",
    "CONFIG_OPTIMIZATION_ASSERTION_LEVEL",
    "CONFIG_OPTIMIZATION_ASSERTIONS_",
    "CONFIG_HAL_DEFAULT_ASSERTION_LEVEL",
    "CONFIG_BOOTLOADER_LOG_LEVEL",
    "LOG_DEFAULT_LEVEL",
]

TARGET_SETTINGS = [
    "CONFIG_IDF_TARGET",
    "CONFIG_IDF_FIRMWARE_CHIP_ID",
    "CONFIG_BOOTLOADER_OFFSET_IN_FLASH",
    "CONFIG_ESP_SLEEP_POWER_DOWN_FLASH",
    "CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE",
    "CONFIG_ESP_SYSTEM_MEMPROT_",
    "CONFIG_ESP_MAIN_TASK_AFFINITY",
    "CONFIG_ESP_WIFI_EXTERNAL_COEXIST_ENABLE",
    "CONFIG_FREERTOS_UNICORE",
    "CONFIG_FREERTOS_TICK_SUPPORT_CORETIMER",
    "CONFIG_FREERTOS_CORETIMER_0",
    "CONFIG_FREERTOS_CORETIMER_1",
    "CONFIG_FREERTOS_SYSTICK_USES_CCOUNT",
    "CONFIG_FREERTOS_OPTIMIZED_SCHEDULER",
    "CONFIG_MBEDTLS_HARDWARE_GCM",
    "CONFIG_ESP_SYSTEM_PD_FLASH",
    "CONFIG_EXTERNAL_COEX_ENABLE",
    "CONFIG_SDK_TOOLPREFIX",
    "CONFIG_TOOLPREFIX",
    "ESP_SLEEP_GPIO_RESET_WORKAROUND",
    "CONFIG_ESP_PHY_ENABLE_USB",
    "CONFIG_BT_SOC_SUPPORT_5_0",
    "CONFIG_NIMBLE_PINNED_TO_CORE",
    "CONFIG_BT_NIMBLE_PINNED_TO_CORE",
    "CONFIG_BT_CTRL_PINNED_TO_CORE",
    "CONFIG_SPIRAM_SPEED_2",
    "CONFIG_SPIRAM_BANKSWITCH_ENABLE",  # For ESP32
]

BOARD_SETTINGS = [
    "CONFIG_LWIP_LOCAL_HOSTNAME",
]

FLASH_SIZE_SETTINGS = [
    "CONFIG_ESPTOOLPY_FLASHSIZE",
    "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME",
    "CONFIG_PARTITION_TABLE_FILENAME",
]

FLASH_MODE_SETTINGS = [
    "CONFIG_ESPTOOLPY_FLASHMODE_",
    "CONFIG_ESPTOOLPY_OCT_FLASH",
    "CONFIG_ESPTOOLPY_FLASH_SAMBLE_MODE_",
]

FLASH_FREQ_SETTINGS = [
    "CONFIG_ESPTOOLPY_FLASHFREQ_",
]

PSRAM_SETTINGS = ["CONFIG_SPIRAM"]

PSRAM_SIZE_SETTINGS = ["CONFIG_SPIRAM_TYPE_"]

PSRAM_MODE_SETTINGS = ["CONFIG_SPIRAM_MODE_"]

PSRAM_FREQ_SETTINGS = ["CONFIG_SPIRAM_SPEED_"]

# Some settings are target dependent but we want to always include them anyway
# because the files they are in will be used across targets.
ALWAYS_INCLUDE = FLASH_MODE_SETTINGS + FLASH_FREQ_SETTINGS + PSRAM_FREQ_SETTINGS

BLE_SETTINGS = ["CONFIG_BT_", "CONFIG_BLUEDROID_", "CONFIG_NIMBLE_", "CONFIG_SW_COEXIST_ENABLE"]

# boards/lilygo_ttgo_t8_s2_st7789/sdkconfig
# CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES=y

# boards/morpheans_morphesp-240/sdkconfig
# CONFIG_LWIP_DNS_SUPPORT_MDNS_QUERIES=y


def matches_group(line, group):
    for setting in group:
        if setting in line:
            return True
    return False


def add_group(lines, last_group, current_group):
    if not current_group or last_group != current_group:
        while last_group and last_group[-1] not in current_group:
            lines.append("# end of " + last_group[-1])
            lines.append("")
            last_group.pop()
        for category in current_group:
            if last_group and category in last_group:
                continue
            lines.append("#")
            lines.append("# " + category)
            lines.append("#")
        return copy.copy(current_group)
    return last_group


def sym_default(sym):
    # Skip symbols that cannot be changed. Only check
    # non-choice symbols, as selects don't affect choice
    # symbols.
    if not sym.choice and sym.visibility <= kconfiglib.expr_value(sym.rev_dep):
        return True

    # Skip symbols whose value matches their default
    if sym.str_value == sym._str_default():
        return True

    # Skip symbols that would be selected by default in a
    # choice, unless the choice is optional or the symbol type
    # isn't bool (it might be possible to set the choice mode
    # to n or the symbol to m in those cases).
    if (
        sym.choice
        and not sym.choice.is_optional
        and sym.choice._selection_from_defaults() is sym
        and sym.orig_type is kconfiglib.BOOL
        and sym.tri_value == 2
    ):
        return True

    return False


@click.command()
@click.option("--debug")
@click.option("--board")
@click.option(
    "--update_all",
    is_flag=True,
    default=False,
    help="Updates the sdkconfigs outside of the board directory.",
)
def update(debug, board, update_all):
    """Updates related sdkconfig files based on the build directory version that
    was likely modified by menuconfig."""

    board_make = pathlib.Path(f"boards/{board}/mpconfigboard.mk")
    psram_size = "0"
    uf2_bootloader = None
    ble_enabled = None
    for line in board_make.read_text().split("\n"):
        if "=" not in line or line.startswith("#"):
            continue
        key, value = line.split("=", maxsplit=1)
        key = key.strip()
        value = value.strip()
        if key == "IDF_TARGET":
            target = value
            if uf2_bootloader is None:
                uf2_bootloader = target not in ("esp32", "esp32c3", "esp32c6", "esp32h2")
            if ble_enabled is None:
                ble_enabled = target not in (
                    "esp32",
                    "esp32s2",
                )  # ESP32 is disabled by us. S2 doesn't support it.
        elif key == "CIRCUITPY_ESP_FLASH_SIZE":
            flash_size = value
        elif key == "CIRCUITPY_ESP_FLASH_MODE":
            flash_mode = value
        elif key == "CIRCUITPY_ESP_FLASH_FREQ":
            flash_freq = value
        elif key == "CIRCUITPY_ESP_PSRAM_SIZE":
            psram_size = value
        elif key == "CIRCUITPY_ESP_PSRAM_MODE":
            psram_mode = value
        elif key == "CIRCUITPY_ESP_PSRAM_FREQ":
            psram_freq = value
        elif key == "UF2_BOOTLOADER":
            uf2_bootloader = not (value == "0")
        elif key == "CIRCUITPY_BLEIO":
            ble_enabled = not (value == "0")

    os.environ["IDF_TARGET"] = target
    os.environ["COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE"] = (
        f"build-{board}/esp-idf/kconfigs_projbuild.in"
    )
    os.environ["COMPONENT_KCONFIGS_SOURCE_FILE"] = f"build-{board}/esp-idf/kconfigs.in"

    kconfig_path = pathlib.Path(f"build-{board}/esp-idf/kconfigs.in")

    kconfig_path = pathlib.Path(f"esp-idf/Kconfig")
    kconfig = kconfiglib.Kconfig(kconfig_path)

    input_config = pathlib.Path(f"build-{board}/esp-idf/sdkconfig")
    kconfig.load_config(input_config)

    sdkconfigs = []
    default_config = pathlib.Path("esp-idf-config/sdkconfig.defaults")
    sdkconfigs.append(default_config)
    if debug:
        opt_config = pathlib.Path("esp-idf-config/sdkconfig-debug.defaults")
    else:
        opt_config = pathlib.Path("esp-idf-config/sdkconfig-opt.defaults")
    sdkconfigs.append(opt_config)
    size_options = ""
    if flash_size == "2MB":
        size_options = "-no-ota-no-uf2"
    elif not uf2_bootloader:
        # These boards don't have native USB.
        size_options = "-no-uf2"
    flash_size_config = pathlib.Path(
        f"esp-idf-config/sdkconfig-flash-{flash_size}{size_options}.defaults"
    )
    flash_mode_config = pathlib.Path(f"esp-idf-config/sdkconfig-flash-{flash_mode}.defaults")
    flash_freq_config = pathlib.Path(f"esp-idf-config/sdkconfig-flash-{flash_freq}.defaults")
    sdkconfigs.extend((flash_size_config, flash_mode_config, flash_freq_config))

    if psram_size != "0":
        psram_config = pathlib.Path(f"esp-idf-config/sdkconfig-psram.defaults")
        psram_size_config = pathlib.Path(f"esp-idf-config/sdkconfig-psram-{psram_size}.defaults")
        psram_mode_config = pathlib.Path(f"esp-idf-config/sdkconfig-psram-{psram_mode}.defaults")
        psram_freq_config = pathlib.Path(f"esp-idf-config/sdkconfig-psram-{psram_freq}.defaults")
        sdkconfigs.extend((psram_config, psram_size_config, psram_mode_config, psram_freq_config))
    target_config = pathlib.Path(f"esp-idf-config/sdkconfig-{target}.defaults")
    sdkconfigs.append(target_config)
    if ble_enabled:
        ble_config = pathlib.Path(f"esp-idf-config/sdkconfig-ble.defaults")
        sdkconfigs.append(ble_config)
    board_config = pathlib.Path(f"boards/{board}/sdkconfig")
    # Don't include the board file in cp defaults. The board may have custom
    # overrides.

    cp_kconfig_defaults = kconfiglib.Kconfig(kconfig_path)
    for default_file in sdkconfigs:
        cp_kconfig_defaults.load_config(default_file, replace=False)

    board_settings = []
    last_board_group = None
    flash_size_settings = []
    last_flash_size_group = None
    flash_mode_settings = []
    flash_freq_settings = []
    psram_settings = []
    last_psram_group = None
    psram_size_settings = []
    psram_mode_settings = []
    psram_freq_settings = []
    opt_settings = []
    last_opt_group = None
    target_settings = []
    last_target_group = None
    ble_settings = []
    last_ble_group = None
    default_settings = []
    last_default_group = None

    target_kconfig_snippets = set()
    target_symbols = set()

    current_group = []

    for sym in kconfig.unique_defined_syms:
        sym._visited = False

    # This merges the normal `write_config`, `write_min_config` and CP settings to split into
    # different files.
    pending_nodes = [kconfig.top_node]
    i = 0
    while pending_nodes:
        node = pending_nodes.pop()
        if node is None:
            current_group.pop()
            continue

        if node.item is kconfiglib.MENU:
            if node.prompt:
                print("  " * len(current_group), i, node.prompt[0])
        i += 1
        if node.next:
            pending_nodes.append(node.next)

        # if i > 300:
        #     break

        # We have a configuration item.
        item = node.item
        if isinstance(item, kconfiglib.Symbol):
            if item._visited:
                continue
            item._visited = True

            config_string = item.config_string.strip()
            if not config_string:
                cp_sym = cp_kconfig_defaults.syms[item.name]
                if cp_sym.str_value == "n":
                    config_string = f"# CONFIG_{item.name} is not set"
                else:
                    continue

            if node.list:
                pending_nodes.append(node.list)

            matches_cp_default = cp_kconfig_defaults.syms[item.name].str_value == item.str_value
            matches_esp_default = sym_default(item)

            print_debug = not matches_esp_default or (not update_all and not matches_cp_default)
            if print_debug:
                print("  " * len(current_group), i, config_string.strip())

            # Some files are `rsource`d into another kconfig with $IDF_TARGET as
            # part of the path. kconfiglib doesn't show this as a reference so
            # we have to look ourselves.
            target_reference = target in item.name_and_loc
            if target_reference:
                loc = item.name_and_loc.split("defined at ")[1].split(":")[0].replace(target, "*")
                if loc not in target_kconfig_snippets:
                    differing_keys = set()
                    shared_keys = {}
                    first = True
                    for path in pathlib.Path(".").glob(loc):
                        kc = kconfiglib.Kconfig(path)
                        all_file_syms = set()
                        for sym in kc.unique_defined_syms:
                            all_file_syms.add(sym)
                            if sym.name in differing_keys:
                                continue
                            if first:
                                shared_keys[sym.name] = sym.str_value
                            elif (
                                sym.name not in shared_keys
                                or shared_keys[sym.name] != sym.str_value
                            ):
                                differing_keys.add(sym.name)
                                if sym.name in shared_keys:
                                    del shared_keys[sym.name]
                        # Any settings missing from a file are *not* shared.
                        shared_syms = set(shared_keys.keys())
                        for missing in shared_syms - all_file_syms:
                            differing_keys.add(missing)
                            del shared_keys[missing]
                        first = False
                    target_kconfig_snippets.add(loc)
                    target_symbols = target_symbols.union(differing_keys)

            # kconfig settings can be set by others. item.referenced doesn't
            # know this. So we collect all things that reference this using
            # rev_dep.
            all_references = set(item.referenced)
            to_unpack = [item.rev_dep]
            while to_unpack:
                rdep = to_unpack.pop()
                if isinstance(rdep, tuple):
                    to_unpack.extend(rdep)
                elif isinstance(rdep, int):
                    # skip logic
                    pass
                else:
                    all_references.add(rdep)
                    all_references.update(rdep.referenced)
            psram_reference = False
            for referenced in all_references:
                if referenced.name.startswith("IDF_TARGET"):
                    target_reference = True
                if referenced.name in target_symbols:
                    # Implicit target symbols
                    target_reference = True
                if referenced.name == "SPIRAM":
                    psram_reference = True

            if (not update_all and not matches_cp_default) or (
                update_all
                and matches_group(config_string, BOARD_SETTINGS)
                and not matches_esp_default
            ):
                print("  " * (len(current_group) + 1), "board")
                last_board_group = add_group(board_settings, last_board_group, current_group)
                board_settings.append(config_string)
            elif update_all:
                target_setting = target_reference or matches_group(config_string, TARGET_SETTINGS)
                if matches_group(config_string, FLASH_SIZE_SETTINGS):
                    print("  " * (len(current_group) + 1), "flash size")
                    last_flash_size_group = add_group(
                        flash_size_settings, last_flash_size_group, current_group
                    )
                    flash_size_settings.append(config_string)
                elif matches_group(config_string, FLASH_MODE_SETTINGS):
                    print("  " * (len(current_group) + 1), "flash mode")
                    flash_mode_settings.append(config_string)
                elif matches_group(config_string, FLASH_FREQ_SETTINGS):
                    print("  " * (len(current_group) + 1), "flash freq")
                    flash_freq_settings.append(config_string)
                elif matches_group(config_string, PSRAM_SIZE_SETTINGS):
                    print("  " * (len(current_group) + 1), "psram size")
                    psram_size_settings.append(config_string)
                elif matches_group(config_string, PSRAM_MODE_SETTINGS):
                    print("  " * (len(current_group) + 1), "psram mode")
                    psram_mode_settings.append(config_string)
                elif (
                    matches_group(config_string, PSRAM_FREQ_SETTINGS)
                    and "26M" not in config_string
                    and "20M" not in config_string
                ):
                    # The ESP32S2 has two frequencies (20M and 26M) that aren't on the S3 or ESP32.
                    # So, put those in target settings.
                    print("  " * (len(current_group) + 1), "psram freq")
                    psram_freq_settings.append(config_string)
                elif matches_esp_default:
                    if print_debug:
                        print("  " * (len(current_group) + 1), "default")
                    # Always document the above settings. Settings below should
                    # be non-default.
                    pass
                elif matches_group(config_string, PSRAM_SETTINGS) or psram_reference:
                    print("  " * (len(current_group) + 1), "psram shared")
                    last_psram_group = add_group(psram_settings, last_psram_group, current_group)
                    psram_settings.append(config_string)
                elif matches_group(config_string, OPT_SETTINGS):
                    print("  " * (len(current_group) + 1), "opt")
                    last_opt_group = add_group(opt_settings, last_opt_group, current_group)
                    opt_settings.append(config_string)
                elif target_setting:
                    print("  " * (len(current_group) + 1), "target")
                    last_target_group = add_group(
                        target_settings, last_target_group, current_group
                    )
                    target_settings.append(config_string)
                elif matches_group(config_string, BLE_SETTINGS):
                    print("  " * (len(current_group) + 1), "ble")
                    last_ble_group = add_group(ble_settings, last_ble_group, current_group)
                    ble_settings.append(config_string)
                else:
                    print("  " * (len(current_group) + 1), "all")
                    last_default_group = add_group(
                        default_settings, last_default_group, current_group
                    )
                    default_settings.append(config_string)

        else:
            if item is kconfiglib.COMMENT:
                print("comment", repr(item))
            elif item is kconfiglib.MENU:
                if node.list:
                    current_group.append(node.prompt[0])
                    pending_nodes.append(None)
                    pending_nodes.append(node.list)
            elif isinstance(item, kconfiglib.Choice):
                # Choices are made up of individual symbols that we need to check.
                pending_nodes.append(node.list)
            else:
                print("unknown", repr(item))

    add_group(board_settings, last_board_group, current_group)
    add_group(opt_settings, last_opt_group, current_group)
    add_group(flash_size_settings, last_flash_size_group, current_group)
    add_group(psram_settings, last_psram_group, current_group)
    add_group(target_settings, last_target_group, current_group)
    add_group(ble_settings, last_ble_group, current_group)
    add_group(default_settings, last_default_group, current_group)

    board_config.write_text("\n".join(board_settings))
    if update_all:
        # Add empty strings to get trailing newlines
        flash_mode_settings.append("")
        flash_freq_settings.append("")
        flash_size_config.write_text("\n".join(flash_size_settings))
        flash_mode_config.write_text("\n".join(flash_mode_settings))
        flash_freq_config.write_text("\n".join(flash_freq_settings))
        if psram_size != "0":
            psram_size_settings.append("")
            psram_mode_settings.append("")
            psram_freq_settings.append("")
            psram_config.write_text("\n".join(psram_settings))
            psram_size_config.write_text("\n".join(psram_size_settings))
            psram_mode_config.write_text("\n".join(psram_mode_settings))
            psram_freq_config.write_text("\n".join(psram_freq_settings))
        opt_config.write_text("\n".join(opt_settings))
        default_config.write_text("\n".join(default_settings))
        target_config.write_text("\n".join(target_settings))
        if ble_settings:
            ble_config.write_text("\n".join(ble_settings))


if __name__ == "__main__":
    update()
