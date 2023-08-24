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
]

BOARD_SETTINGS = [
    "CONFIG_SPIRAM",
    "CONFIG_DEFAULT_PSRAM_",
    "_SPIRAM_SUPPORT",
    "CONFIG_LWIP_LOCAL_HOSTNAME",
]

FLASH_SETTINGS = [
    "CONFIG_ESPTOOLPY_FLASHSIZE",
    "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME",
    "CONFIG_PARTITION_TABLE_FILENAME",
]

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
    for line in board_make.read_text().split("\n"):
        if line.startswith("IDF_TARGET"):
            target = line.split("=")[1].strip()
        elif line.startswith("CIRCUITPY_ESP_FLASH_SIZE"):
            flash = line.split("=")[1].strip()

    os.environ["IDF_TARGET"] = target
    os.environ[
        "COMPONENT_KCONFIGS_PROJBUILD_SOURCE_FILE"
    ] = f"build-{board}/esp-idf/kconfigs_projbuild.in"
    os.environ["COMPONENT_KCONFIGS_SOURCE_FILE"] = f"build-{board}/esp-idf/kconfigs.in"

    kconfig_path = pathlib.Path(f"build-{board}/esp-idf/kconfigs.in")

    kconfig_path = pathlib.Path(f"esp-idf/Kconfig")
    kconfig = kconfiglib.Kconfig(kconfig_path)

    input_config = pathlib.Path(f"build-{board}/esp-idf/sdkconfig")
    kconfig.load_config(input_config)

    default_config = pathlib.Path("esp-idf-config/sdkconfig.defaults")
    if debug:
        opt_config = pathlib.Path("esp-idf-config/sdkconfig-debug.defaults")
    else:
        opt_config = pathlib.Path("esp-idf-config/sdkconfig-opt.defaults")
    flash_config = pathlib.Path(f"esp-idf-config/sdkconfig-{flash}.defaults")
    target_config = pathlib.Path(f"esp-idf-config/sdkconfig-{target}.defaults")
    ble_config = pathlib.Path(f"esp-idf-config/sdkconfig-ble.defaults")
    board_config = pathlib.Path(f"boards/{board}/sdkconfig")

    cp_kconfig_defaults = kconfiglib.Kconfig(kconfig_path)
    for default_file in (default_config, opt_config, flash_config, target_config, ble_config):
        cp_kconfig_defaults.load_config(default_file, replace=False)

    board_settings = []
    last_board_group = None
    flash_settings = []
    last_flash_group = None
    opt_settings = []
    last_opt_group = None
    target_settings = []
    last_target_group = None
    ble_settings = []
    last_ble_group = None
    default_settings = []
    last_default_group = None

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
                continue

            if node.list:
                pending_nodes.append(node.list)

            matches_cp_default = cp_kconfig_defaults.syms[item.name].str_value == item.str_value
            matches_esp_default = sym_default(item)

            if not matches_esp_default:
                print("  " * len(current_group), i, config_string.strip())

            target_reference = False
            board_reference = False
            for referenced in item.referenced:
                if referenced.name.startswith("IDF_TARGET"):
                    # print(item.name, "references", referenced.name)
                    target_reference = True
                    break
                if referenced.name == "SPIRAM":
                    board_reference = True

            if (not update_all and not matches_cp_default) or (
                update_all
                and (matches_group(config_string, BOARD_SETTINGS) or board_reference)
                and not matches_esp_default
            ):
                print("  " * (len(current_group) + 1), "board")
                last_board_group = add_group(board_settings, last_board_group, current_group)
                board_settings.append(config_string)
            elif update_all and not matches_esp_default:
                if matches_group(config_string, OPT_SETTINGS):
                    print("  " * (len(current_group) + 1), "opt")
                    last_opt_group = add_group(opt_settings, last_opt_group, current_group)
                    opt_settings.append(config_string)
                elif matches_group(config_string, FLASH_SETTINGS):
                    print("  " * (len(current_group) + 1), "flash")
                    last_flash_group = add_group(flash_settings, last_flash_group, current_group)
                    flash_settings.append(config_string)
                elif target_reference or matches_group(config_string, TARGET_SETTINGS):
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

        elif kconfiglib.expr_value(node.dep):
            if item is kconfiglib.COMMENT:
                print("comment", repr(item))
            elif item is kconfiglib.MENU:
                # This menu isn't visible so skip to the next node.
                if kconfiglib.expr_value(node.visibility) and node.list:
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
    add_group(flash_settings, last_flash_group, current_group)
    add_group(target_settings, last_target_group, current_group)
    add_group(ble_settings, last_ble_group, current_group)
    add_group(default_settings, last_default_group, current_group)

    board_config.write_text("\n".join(board_settings))
    if update_all:
        flash_config.write_text("\n".join(flash_settings))
        opt_config.write_text("\n".join(opt_settings))
        default_config.write_text("\n".join(default_settings))
        target_config.write_text("\n".join(target_settings))
        ble_config.write_text("\n".join(ble_settings))


if __name__ == "__main__":
    update()
