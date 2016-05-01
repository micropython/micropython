
#include <stddef.h>
#include "platform_config.h"

#include "platforms.h"
#include "usbserial_api.h"
#include "usartserial_api.h"
#include "tcpserver_api.h"
#include "tcpclient_api.h"
#include "wifi_api.h"
#include "gpio_api.h"
#include "ticks_api.h"
#include "timer_api.h"
#include "interrupt_api.h"
#include "spi_api.h"
#include "i2c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void mp_setup(void);
void mp_loop(void);


#ifdef __cplusplus
}
#endif
