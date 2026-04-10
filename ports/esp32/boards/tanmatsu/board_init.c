
void tanmatsu_board_startup(void) {
    // Initialize USB-Serial/JTAG like the example
    //usb_serial_jtag_driver_config_t cfg = USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT();
    //usb_serial_jtag_driver_install(&cfg);

    // Route stdin/stdout/stderr to USB console
    //esp_vfs_usb_serial_jtag_use_driver();
}
