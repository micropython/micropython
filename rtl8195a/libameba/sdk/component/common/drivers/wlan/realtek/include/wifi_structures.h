#ifndef _WIFI_STRUCTURES_H
#define _WIFI_STRUCTURES_H

//#include <freertos/freertos_service.h>
#include "wifi_constants.h"
#ifdef	__cplusplus
extern "C" {
#endif

#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack(1)
#endif
typedef struct rtw_ssid {
    unsigned char len;     /**< SSID length */
    unsigned char val[33]; /**< SSID name (AP name)  */
} rtw_ssid_t;
#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack()
#endif

#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack(1)
#endif
typedef struct rtw_mac {
    unsigned char octet[6]; /**< Unique 6-byte MAC address */
} rtw_mac_t;
#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack()
#endif

typedef struct rtw_ap_info {
	rtw_ssid_t 			ssid;
	rtw_security_t		security_type;
	unsigned char 		*password;
	int 				password_len;
	int					channel;
}rtw_ap_info_t;

typedef struct rtw_network_info {
	rtw_ssid_t 			ssid;
	rtw_mac_t			bssid;
	rtw_security_t		security_type;
	unsigned char 		*password;
	int 				password_len;
	int					key_id;
}rtw_network_info_t;

#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack(1)
#endif
typedef struct rtw_scan_result {
    rtw_ssid_t              SSID;             /**< Service Set Identification (i.e. Name of Access Point)                    */
    rtw_mac_t               BSSID;            /**< Basic Service Set Identification (i.e. MAC address of Access Point)       */
    signed short		                  signal_strength;  /**< Receive Signal Strength Indication in dBm. <-90=Very poor, >-30=Excellent */
    rtw_bss_type_t          bss_type;         /**< Network type                                                              */
    rtw_security_t          security;         /**< Security type                                                             */
    rtw_wps_type_t          wps_type;         /**< WPS type                                                                  */
    unsigned int                      channel;          /**< Radio channel that the AP beacon was received on                          */
    rtw_802_11_band_t       band;             /**< Radio band                                                                */                                        
} rtw_scan_result_t;
#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack()
#endif

typedef struct rtw_scan_handler_result {
    rtw_scan_result_t ap_details;
    rtw_bool_t        scan_complete;
    void*               user_data;

} rtw_scan_handler_result_t;

#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack(1)
#endif
typedef struct rtw_wifi_setting {
	rtw_mode_t		mode;
	unsigned char 		ssid[33];
	unsigned char		channel;
	rtw_security_t		security_type;
	unsigned char 		password[65];
	unsigned char		key_idx;
}rtw_wifi_setting_t;
#if defined(__IAR_SYSTEMS_ICC__)
#pragma pack()
#endif

typedef struct rtw_wifi_config {
	unsigned int		boot_mode;
	unsigned char 		ssid[32];
	unsigned char		ssid_len;
	unsigned char		security_type;
	unsigned char		password[65];
	unsigned char		password_len;
	unsigned char		channel;
} rtw_wifi_config_t;

typedef struct
{
    unsigned int    count;         /**< Number of MAC addresses in the list    */
    rtw_mac_t mac_list[1];   /**< Variable length array of MAC addresses */
} rtw_maclist_t;

typedef struct {
    unsigned int      version;                /* version field */
    unsigned int      length;                 /* byte length of data in this record,   */
                                          /* starting at version and including IEs */
    rtw_mac_t     BSSID;
    unsigned short      beacon_period;          /* units are Kusec */
    unsigned short      capability;             /* Capability information */
    unsigned char       SSID_len;
    unsigned char       SSID[32];
    unsigned char       channel;
//    struct {
//        uint32_t    count;                /* # rates in this set */
//        uint8_t   rates[16];              /* rates in 500kbps units w/hi bit set if basic */
//    } rateset;                            /* supported rates */
//    rtw_chanspec_t  chanspec;              /* chanspec for bss */
    unsigned short      atim_window;            /* units are Kusec */
    unsigned char       dtim_period;            /* DTIM period */
    signed short       RSSI;                   /* receive signal strength (in dBm) */

    unsigned char       n_cap;                  /* BSS is 802.11N Capable */
    unsigned int      nbss_cap;               /* 802.11N BSS Capabilities (based on HT_CAP_*) */
    unsigned char       basic_mcs[MCSSET_LEN];  /* 802.11N BSS required MCS set */

    unsigned short      ie_offset;              /* offset at which IEs start, from beginning */
    unsigned int      ie_length;              /* byte length of Information Elements */
} rtw_bss_info_t;

typedef struct {
	unsigned short	offset;     /**< Offset in bytes to start filtering (referenced to the start of the ethernet packet) */
	unsigned short	mask_size;  /**< Size of the mask in bytes */
	unsigned char*	mask;       /**< Pattern mask bytes to be ANDed with the pattern eg. "\xff00" (must be in network byte order) */
	unsigned char*	pattern;    /**< Pattern bytes used to filter eg. "\x0800"  (must be in network byte order) */
} rtw_packet_filter_pattern_t;

typedef struct ieee80211_frame_info{
	unsigned short i_fc;
	unsigned short i_dur;
	unsigned char i_addr1[6];
	unsigned char i_addr2[6];
	unsigned char i_addr3[6];
	unsigned short i_seq;
	unsigned char bssid[6];
	unsigned char encrypt;
	signed char rssi;
}ieee80211_frame_info_t;

typedef struct {
	char filter_id;
	rtw_packet_filter_pattern_t patt;
	rtw_packet_filter_rule_e rule;
	unsigned char enable;
}rtw_packet_filter_info_t;
#ifdef	__cplusplus
}
#endif

#endif /* _WIFI_STRUCTURES_H */
