#ifndef MICROPY_INCLUDED_PORTS_UNIX_BTSTACK_CONFIG_H
#define MICROPY_INCLUDED_PORTS_UNIX_BTSTACK_CONFIG_H


#include "extmod/btstack/btstack_config_common.h"

#ifdef MICROPY_BLUETOOTH_BTSTACK_USB
#if MICROPY_BLUETOOTH_BTSTACK_USB

// Workaround:
//
// If ENABLE_SCO_OVER_HCI is not defined there is a compiler error:
//
// ../../lib/btstack/platform/libusb/hci_transport_h2_libusb.c:1354:13:
// error: ‘signal_sco_can_send_now’ defined but not used
//
// Because the problem is in an external code this workaround is done.
#define ENABLE_SCO_OVER_HCI

#endif
#endif

#endif // MICROPY_INCLUDED_PORTS_UNIX_BTSTACK_CONFIG_H
