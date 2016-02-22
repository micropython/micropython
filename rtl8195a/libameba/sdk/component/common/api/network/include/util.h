#ifndef _UTIL_H
#define _UTIL_H

#include <wireless.h>
#include <wlan_intf.h>

#ifdef	__cplusplus
extern "C" {
#endif

#include "wifi_util.h"
#if 0
typedef enum _WIFI_EVENT_INDICATE{
	WIFI_EVENT_CONNECT = 0,
	WIFI_EVENT_DISCONNECT = 1,
	WIFI_EVENT_FOURWAY_HANDSHAKE_DONE = 2,
}WIFI_EVENT_INDICATE;

int wext_get_ssid(const char *ifname, __u8 *ssid);
int wext_set_ssid(const char *ifname, const __u8 *ssid, __u16 ssid_len);
int wext_set_auth_param(const char *ifname, __u16 idx, __u32 value);
int wext_set_key_ext(const char *ifname, __u16 alg, const __u8 *addr, int key_idx, int set_tx, const __u8 *seq, __u16 seq_len, __u8 *key, __u16 key_len);
int wext_get_enc_ext(const char *ifname, __u16 *alg);
int wext_set_passphrase(const char *ifname, const __u8 *passphrase, __u16 passphrase_len);
int wext_get_passphrase(const char *ifname, __u8 *passphrase);
int wext_set_mode(const char *ifname, int mode);
int wext_get_mode(const char *ifname, int *mode);
int wext_set_ap_ssid(const char *ifname, const __u8 *ssid, __u16 ssid_len);
int wext_set_country(const char *ifname, char *country_code);
int wext_get_rssi(const char *ifname, int *rssi);
int wext_set_channel(const char *ifname, __u8 ch);
int wext_get_channel(const char *ifname, __u8 *ch);
int wext_set_scan(const char *ifname, char *buf, __u16 buf_len);
int wext_get_scan(const char *ifname, char *buf, __u16 buf_len);
int wext_mp_command(const char *ifname, char *cmd, int show_msg);
int wext_wifi_priv(const char *ifname, int argc, char **argv);
void wext_wlan_indicate(unsigned int cmd, union iwreq_data *wrqu, char *extra);
#endif

#define wext_handshake_done rltk_wlan_handshake_done

#ifdef	__cplusplus
}
#endif

#endif /* _UTIL_H */
