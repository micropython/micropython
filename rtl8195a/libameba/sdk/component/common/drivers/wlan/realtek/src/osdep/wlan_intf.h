#ifndef __WLAN_INTF_H__
#define __WLAN_INTF_H__

#ifdef	__cplusplus
extern "C" {
#endif
#include <autoconf.h>

#include <wireless.h>
#include "wifi_constants.h"

#ifndef WLAN0_IDX
	#define WLAN0_IDX	0
#endif
#ifndef WLAN1_IDX
	#define WLAN1_IDX	1
#endif
#ifndef WLAN_UNDEF
	#define WLAN_UNDEF	-1
#endif

/***********************************************************/
/* 
struct sk_buff {
	// These two members must be first.
	struct sk_buff		*next;		// Next buffer in list
	struct sk_buff		*prev;		// Previous buffer in list
	
	struct sk_buff_head	*list;			// List we are on	
	unsigned char		*head;		// Head of buffer
	unsigned char		*data;		// Data head pointer
	unsigned char		*tail;		// Tail pointer
	unsigned char		*end;		//End pointer
	struct net_device 	*dev;		//Device we arrived on/are leaving by 	
	unsigned int 		len;			// Length of actual data 
};
*/
/************************************************************/

//----- ------------------------------------------------------------------
// Wlan Interface opened for upper layer
//----- ------------------------------------------------------------------
int rltk_wlan_init(int idx_wlan, rtw_mode_t mode);				//return 0: success. -1:fail
void rltk_wlan_deinit(void);
void rltk_wlan_start(int idx_wlan);
void rltk_wlan_statistic(unsigned char idx);
unsigned char rltk_wlan_running(unsigned char idx);		// interface is up. 0: interface is down
int rltk_wlan_control(unsigned long cmd, void *data);
int rltk_wlan_handshake_done(void);
int rltk_wlan_rf_on(void);
int rltk_wlan_rf_off(void);
int rltk_wlan_check_bus(void);
int rltk_wlan_wireless_mode(unsigned char mode);
int rltk_wlan_set_wps_phase(unsigned char is_trigger_wps);
int rtw_ps_enable(int enable);
int rltk_wlan_is_connected_to_ap(void);


#ifdef	__cplusplus
}
#endif



#endif //#ifndef __WLAN_INTF_H__
