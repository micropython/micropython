#ifndef _WIFI_INDICATE_H
#define _WIFI_INDICATE_H
#include "wifi_conf.h"  
#if 0 //move to wifi_constants.h 
typedef enum _WIFI_EVENT_INDICATE{
	WIFI_EVENT_CONNECT = 0,
	WIFI_EVENT_DISCONNECT = 1,
	WIFI_EVENT_FOURWAY_HANDSHAKE_DONE = 2,	
	WIFI_EVENT_SCAN_RESULT_REPORT = 3,
	WIFI_EVENT_SCAN_DONE = 4,
	WIFI_EVENT_RECONNECTION_FAIL = 5,
	WIFI_EVENT_SEND_ACTION_DONE = 6,
	WIFI_EVENT_RX_MGNT = 7,
	WIFI_EVENT_STA_ASSOC = 8,
	WIFI_EVENT_STA_DISASSOC = 9,
	WIFI_EVENT_WPS_FINISH = 10,
	WIFI_EVENT_EAPOL_RECVD = 11,
	WIFI_EVENT_NO_NETWORK = 12,
	WIFI_EVENT_BEACON_AFTER_DHCP = 13,
	WIFI_EVENT_MAX,
}WIFI_EVENT_INDICATE;
#endif
typedef void (*rtw_event_handler_t)(char *buf, int buf_len, int flags, void* handler_user_data );

typedef struct
{
//	WIFI_EVENT_INDICATE	event_cmd;
	rtw_event_handler_t	handler;
	void*	handler_user_data;
} event_list_elem_t;

void init_event_callback_list(void);
extern void wifi_indication( WIFI_EVENT_INDICATE event, char *buf, int buf_len, int flags);
/** Register the event listener
 *
 * @param[in] event_cmds : The event command number indicated
 * @param[in] handler_func : the callback function which will 
 *  			  receive and process the event
 * @param[in] handler_user_data : user specific data that will be 
 *  			   passed directly to the callback function
 *  
 * @note : Set the same event_cmds with empty handler_func will
 *  	 unregister the event_cmds
 *  
 * @return    RTW_SUCCESS : if successfully registers the event 
 *  		  RTW_ERROR : if an error occurred
 */
extern void wifi_reg_event_handler(unsigned int event_cmds, rtw_event_handler_t handler_func, void *handler_user_data);
extern void wifi_unreg_event_handler(unsigned int event_cmds, rtw_event_handler_t handler_func);

#endif //_WIFI_INDICATE_H

