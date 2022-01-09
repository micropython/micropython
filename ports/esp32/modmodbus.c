#include "modmodbus.h"

#include "esp_modbus_common.h"
#include "esp_modbus_slave.h"
#include "uart.h"

static char area_names[][9] = {"HOLDING", "INPUT", "COIL", "DISCRETE"};
static char parity_names[][6] = {"NONE", "ERROR", "EVEN", "ODD"};
static char serial_mode_names[][5] = {"RTU", "ASCII"};

static portMUX_TYPE modbus_spinlock = portMUX_INITIALIZER_UNLOCKED;

