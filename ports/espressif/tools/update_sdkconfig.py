"""This script updates the sdkconfigs based on the menuconfig results in a given
   build."""

import pathlib
import click

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
]

TARGET_SETTINGS = [
    "CONFIG_IDF_TARGET",
    "CONFIG_IDF_FIRMWARE_CHIP_ID",
    "CONFIG_BOOTLOADER_OFFSET_IN_FLASH",
    "CONFIG_ESP32_",
    "CONFIG_ESP32C3_",
    "CONFIG_ESP32S2_",
    "CONFIG_ESP32S3_",
    "CONFIG_ESP32H2_",
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
]

BOARD_SETTINGS = [
    "PSRAM clock and cs IO for ESP32S3",
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
    # TODO: Properly handle nested groups
    if last_group != current_group[-1]:
        if last_group:
            lines.append("# end of " + last_group)
            lines.append("")
            return None
        if current_group:
            lines.append("#")
            lines.append("# " + current_group[-1])
            lines.append("#")
            return current_group[-1]
    return last_group


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
    """Simple program that greets NAME for a total of COUNT times."""

    board_make = pathlib.Path(f"boards/{board}/mpconfigboard.mk")
    for line in board_make.read_text().split("\n"):
        if line.startswith("IDF_TARGET"):
            target = line.split("=")[1].strip()
        elif line.startswith("CIRCUITPY_ESP_FLASH_SIZE"):
            flash = line.split("=")[1].strip()

    input_config = pathlib.Path(f"build-{board}/esp-idf/sdkconfig")
    default_config = pathlib.Path("esp-idf-config/sdkconfig.defaults")
    if debug:
        opt_config = pathlib.Path("esp-idf-config/sdkconfig-debug.defaults")
    else:
        opt_config = pathlib.Path("esp-idf-config/sdkconfig-opt.defaults")
    flash_config = pathlib.Path(f"esp-idf-config/sdkconfig-{flash}.defaults")
    target_config = pathlib.Path(f"esp-idf-config/sdkconfig-{target}.defaults")
    ble_config = pathlib.Path(f"esp-idf-config/sdkconfig-ble.defaults")
    board_config = pathlib.Path(f"boards/{board}/sdkconfig")

    defaults = default_config.read_text().split("\n")
    defaults.extend(opt_config.read_text().split("\n"))
    defaults.extend(flash_config.read_text().split("\n"))
    defaults.extend(target_config.read_text().split("\n"))
    defaults.extend(ble_config.read_text().split("\n"))

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
    for line in input_config.read_text().split("\n"):
        if line.startswith("# ") and "CONFIG_" not in line and len(line) > 3:
            if line.startswith("# end of"):
                current_group.pop()
            else:
                current_group.append(line[2:])
        elif (not update_all and line not in defaults) or (
            update_all and matches_group(line, BOARD_SETTINGS)
        ):
            last_board_group = add_group(board_settings, last_board_group, current_group)
            board_settings.append(line)
        elif update_all:
            if matches_group(line, OPT_SETTINGS):
                last_opt_group = add_group(opt_settings, last_opt_group, current_group)
                opt_settings.append(line)
            elif matches_group(line, FLASH_SETTINGS):
                last_flash_group = add_group(flash_settings, last_flash_group, current_group)
                flash_settings.append(line)
            elif matches_group(line, TARGET_SETTINGS):
                last_target_group = add_group(target_settings, last_target_group, current_group)
                target_settings.append(line)
            elif matches_group(line, BLE_SETTINGS):
                last_ble_group = add_group(ble_settings, last_ble_group, current_group)
                ble_settings.append(line)
            elif "CONFIG_" in line:
                last_default_group = add_group(default_settings, last_default_group, current_group)
                default_settings.append(line)

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
