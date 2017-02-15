#ifndef _WIFI_CONSTANTS_H
#define _WIFI_CONSTANTS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define WEP_ENABLED        0x0001
#define TKIP_ENABLED       0x0002
#define AES_ENABLED        0x0004
#define WSEC_SWFLAG        0x0008

#define SHARED_ENABLED  0x00008000
#define WPA_SECURITY    0x00200000
#define WPA2_SECURITY   0x00400000
#define WPS_ENABLED     0x10000000

#define RTW_MAX_PSK_LEN		(64)
#define RTW_MIN_PSK_LEN		(8)

#define MCSSET_LEN			16

typedef enum
{
    RTW_SUCCESS                      = 0,    /**< Success */
    RTW_PENDING                      = 1,    /**< Pending */
    RTW_TIMEOUT                      = 2,    /**< Timeout */
    RTW_PARTIAL_RESULTS              = 3,    /**< Partial results */
    RTW_INVALID_KEY                  = 4,    /**< Invalid key */
    RTW_DOES_NOT_EXIST               = 5,    /**< Does not exist */
    RTW_NOT_AUTHENTICATED            = 6,    /**< Not authenticated */
    RTW_NOT_KEYED                    = 7,    /**< Not keyed */
    RTW_IOCTL_FAIL                   = 8,    /**< IOCTL fail */
    RTW_BUFFER_UNAVAILABLE_TEMPORARY = 9,    /**< Buffer unavailable temporarily */
    RTW_BUFFER_UNAVAILABLE_PERMANENT = 10,   /**< Buffer unavailable permanently */
    RTW_WPS_PBC_OVERLAP              = 11,   /**< WPS PBC overlap */
    RTW_CONNECTION_LOST              = 12,   /**< Connection lost */

    RTW_ERROR                        = -1,   /**< Generic Error */
    RTW_BADARG                       = -2,   /**< Bad Argument */
    RTW_BADOPTION                    = -3,   /**< Bad option */
    RTW_NOTUP                        = -4,   /**< Not up */
    RTW_NOTDOWN                      = -5,   /**< Not down */
    RTW_NOTAP                        = -6,   /**< Not AP */
    RTW_NOTSTA                       = -7,   /**< Not STA  */
    RTW_BADKEYIDX                    = -8,   /**< BAD Key Index */
    RTW_RADIOOFF                     = -9,   /**< Radio Off */
    RTW_NOTBANDLOCKED                = -10,  /**< Not  band locked */
    RTW_NOCLK                        = -11,  /**< No Clock */
    RTW_BADRATESET                   = -12,  /**< BAD Rate valueset */
    RTW_BADBAND                      = -13,  /**< BAD Band */
    RTW_BUFTOOSHORT                  = -14,  /**< Buffer too short */
    RTW_BUFTOOLONG                   = -15,  /**< Buffer too long */
    RTW_BUSY                         = -16,  /**< Busy */
    RTW_NOTASSOCIATED                = -17,  /**< Not Associated */
    RTW_BADSSIDLEN                   = -18,  /**< Bad SSID len */
    RTW_OUTOFRANGECHAN               = -19,  /**< Out of Range Channel */
    RTW_BADCHAN                      = -20,  /**< Bad Channel */
    RTW_BADADDR                      = -21,  /**< Bad Address */
    RTW_NORESOURCE                   = -22,  /**< Not Enough Resources */
    RTW_UNSUPPORTED                  = -23,  /**< Unsupported */
    RTW_BADLEN                       = -24,  /**< Bad length */
    RTW_NOTREADY                     = -25,  /**< Not Ready */
    RTW_EPERM                        = -26,  /**< Not Permitted */
    RTW_NOMEM                        = -27,  /**< No Memory */
    RTW_ASSOCIATED                   = -28,  /**< Associated */
    RTW_RANGE                        = -29,  /**< Not In Range */
    RTW_NOTFOUND                     = -30,  /**< Not Found */
    RTW_WME_NOT_ENABLED              = -31,  /**< WME Not Enabled */
    RTW_TSPEC_NOTFOUND               = -32,  /**< TSPEC Not Found */
    RTW_ACM_NOTSUPPORTED             = -33,  /**< ACM Not Supported */
    RTW_NOT_WME_ASSOCIATION          = -34,  /**< Not WME Association */
    RTW_SDIO_ERROR                   = -35,  /**< SDIO Bus Error */
    RTW_WLAN_DOWN                    = -36,  /**< WLAN Not Accessible */
    RTW_BAD_VERSION                  = -37,  /**< Incorrect version */
    RTW_TXFAIL                       = -38,  /**< TX failure */
    RTW_RXFAIL                       = -39,  /**< RX failure */
    RTW_NODEVICE                     = -40,  /**< Device not present */
    RTW_UNFINISHED                   = -41,  /**< To be finished */
    RTW_NONRESIDENT                  = -42,  /**< access to nonresident overlay */
    RTW_DISABLED                     = -43   /**< Disabled in this build */
} rtw_result_t;

typedef enum {
    RTW_SECURITY_OPEN           = 0,                                                /**< Open security                           */
    RTW_SECURITY_WEP_PSK        = WEP_ENABLED,                                      /**< WEP Security with open authentication   */
    RTW_SECURITY_WEP_SHARED     = ( WEP_ENABLED | SHARED_ENABLED ),                 /**< WEP Security with shared authentication */
    RTW_SECURITY_WPA_TKIP_PSK   = ( WPA_SECURITY  | TKIP_ENABLED ),                 /**< WPA Security with TKIP                  */
    RTW_SECURITY_WPA_AES_PSK    = ( WPA_SECURITY  | AES_ENABLED ),                  /**< WPA Security with AES                   */
    RTW_SECURITY_WPA2_AES_PSK   = ( WPA2_SECURITY | AES_ENABLED ),                  /**< WPA2 Security with AES                  */
    RTW_SECURITY_WPA2_TKIP_PSK  = ( WPA2_SECURITY | TKIP_ENABLED ),                 /**< WPA2 Security with TKIP                 */
    RTW_SECURITY_WPA2_MIXED_PSK = ( WPA2_SECURITY | AES_ENABLED | TKIP_ENABLED ),   /**< WPA2 Security with AES & TKIP           */
    RTW_SECURITY_WPA_WPA2_MIXED = ( WPA_SECURITY  | WPA2_SECURITY ),                /**< WPA/WPA2 Security                       */

    RTW_SECURITY_WPS_OPEN       = WPS_ENABLED,                                      /**< WPS with open security                  */
    RTW_SECURITY_WPS_SECURE     = (WPS_ENABLED | AES_ENABLED),                      /**< WPS with AES security                   */

    RTW_SECURITY_UNKNOWN        = -1,                                               /**< May be returned by scan function if security is unknown. Do not pass this to the join function! */

    RTW_SECURITY_FORCE_32_BIT   = 0x7fffffff                                        /**< Exists only to force rtw_security_t type to 32 bits */
} rtw_security_t;

typedef enum {
    RTW_ENCRYPTION_UNKNOWN = 0,                 
    RTW_ENCRYPTION_OPEN = 1,
    RTW_ENCRYPTION_WEP40 = 2,
    RTW_ENCRYPTION_WPA_TKIP = 3,
    RTW_ENCRYPTION_WPA_AES = 4,
    RTW_ENCRYPTION_WPA2_TKIP = 5,
    RTW_ENCRYPTION_WPA2_AES = 6,
    RTW_ENCRYPTION_WPA2_MIXED = 7,
    RTW_ENCRYPTION_WEP104 = 9,
    RTW_ENCRYPTION_UNDEF = 0xFF,                 
} rtw_encryption_t;

typedef enum {
	RTW_FALSE = 0,
	RTW_TRUE  = 1
} rtw_bool_t;

typedef enum {
	RTW_802_11_BAND_5GHZ   = 0, /**< Denotes 5GHz radio band   */
	RTW_802_11_BAND_2_4GHZ = 1  /**< Denotes 2.4GHz radio band */
} rtw_802_11_band_t;

typedef enum {


	/* SPECIAL */
	RTW_COUNTRY_WORLD,
	RTW_COUNTRY_EU,

	/* JAPANESE */
	RTW_COUNTRY_JP,

	/* FCC , 19 countries*/
	RTW_COUNTRY_AS,
	RTW_COUNTRY_BM,
	RTW_COUNTRY_CA,
	RTW_COUNTRY_DM,
	RTW_COUNTRY_DO,
	RTW_COUNTRY_FM,
	RTW_COUNTRY_GD,
	RTW_COUNTRY_GT,
	RTW_COUNTRY_GU,
	RTW_COUNTRY_HT,
	RTW_COUNTRY_MH,
	RTW_COUNTRY_MP,
	RTW_COUNTRY_NI,
	RTW_COUNTRY_PA,
	RTW_COUNTRY_PR,
	RTW_COUNTRY_PW,
	RTW_COUNTRY_TW,
	RTW_COUNTRY_US,
	RTW_COUNTRY_VI,

	/* others,  ETSI */
	RTW_COUNTRY_AD,
	RTW_COUNTRY_AE,
	RTW_COUNTRY_AF,
	RTW_COUNTRY_AI,
	RTW_COUNTRY_AL,
	RTW_COUNTRY_AM,
	RTW_COUNTRY_AN,
	RTW_COUNTRY_AR,
	RTW_COUNTRY_AT,
	RTW_COUNTRY_AU,
	RTW_COUNTRY_AW,
	RTW_COUNTRY_AZ,
	RTW_COUNTRY_BA,
	RTW_COUNTRY_BB,
	RTW_COUNTRY_BD,
	RTW_COUNTRY_BE,
	RTW_COUNTRY_BF,
	RTW_COUNTRY_BG,
	RTW_COUNTRY_BH,
	RTW_COUNTRY_BL,
	RTW_COUNTRY_BN,
	RTW_COUNTRY_BO,
	RTW_COUNTRY_BR,
	RTW_COUNTRY_BS,
	RTW_COUNTRY_BT,
	RTW_COUNTRY_BY,
	RTW_COUNTRY_BZ,
	RTW_COUNTRY_CF,
	RTW_COUNTRY_CH,
	RTW_COUNTRY_CI,
	RTW_COUNTRY_CL,
	RTW_COUNTRY_CN,
	RTW_COUNTRY_CO,
	RTW_COUNTRY_CR,
	RTW_COUNTRY_CX,
	RTW_COUNTRY_CY,
	RTW_COUNTRY_CZ,
	RTW_COUNTRY_DE,
	RTW_COUNTRY_DK,
	RTW_COUNTRY_DZ,
	RTW_COUNTRY_EC,
	RTW_COUNTRY_EE,
	RTW_COUNTRY_EG,
	RTW_COUNTRY_ES,
	RTW_COUNTRY_ET,
	RTW_COUNTRY_FI,
	RTW_COUNTRY_FR,
	RTW_COUNTRY_GB,
	RTW_COUNTRY_GE,
	RTW_COUNTRY_GF,
	RTW_COUNTRY_GH,
	RTW_COUNTRY_GL,
	RTW_COUNTRY_GP,
	RTW_COUNTRY_GR,
	RTW_COUNTRY_GY,
	RTW_COUNTRY_HK,
	RTW_COUNTRY_HN,
	RTW_COUNTRY_HR,
	RTW_COUNTRY_HU,
	RTW_COUNTRY_ID,
	RTW_COUNTRY_IE,
	RTW_COUNTRY_IL,
	RTW_COUNTRY_IN,
	RTW_COUNTRY_IQ,
	RTW_COUNTRY_IR,
	RTW_COUNTRY_IS,
	RTW_COUNTRY_IT,
	RTW_COUNTRY_JM,
	RTW_COUNTRY_JO,
	RTW_COUNTRY_KE,
	RTW_COUNTRY_KH,
	RTW_COUNTRY_KN,
	RTW_COUNTRY_KP,
	RTW_COUNTRY_KR,
	RTW_COUNTRY_KW,
	RTW_COUNTRY_KY,
	RTW_COUNTRY_KZ,
	RTW_COUNTRY_LA,
	RTW_COUNTRY_LB,
	RTW_COUNTRY_LC,
	RTW_COUNTRY_LI,
	RTW_COUNTRY_LK,
	RTW_COUNTRY_LR,
	RTW_COUNTRY_LS,
	RTW_COUNTRY_LT,
	RTW_COUNTRY_LU,
	RTW_COUNTRY_LV,
	RTW_COUNTRY_MA,
	RTW_COUNTRY_MC,
	RTW_COUNTRY_MD,
	RTW_COUNTRY_ME,
	RTW_COUNTRY_MF,
	RTW_COUNTRY_MK,
	RTW_COUNTRY_MN,
	RTW_COUNTRY_MO,
	RTW_COUNTRY_MQ,
	RTW_COUNTRY_MR,
	RTW_COUNTRY_MT,
	RTW_COUNTRY_MU,
	RTW_COUNTRY_MV,
	RTW_COUNTRY_MW,
	RTW_COUNTRY_MX,
	RTW_COUNTRY_MY,
	RTW_COUNTRY_NG,
	RTW_COUNTRY_NL,
	RTW_COUNTRY_NO,
	RTW_COUNTRY_NP,
	RTW_COUNTRY_NZ,
	RTW_COUNTRY_OM,
	RTW_COUNTRY_PE,
	RTW_COUNTRY_PF,
	RTW_COUNTRY_PG,
	RTW_COUNTRY_PH,
	RTW_COUNTRY_PK,
	RTW_COUNTRY_PL,
	RTW_COUNTRY_PM,
	RTW_COUNTRY_PT,
	RTW_COUNTRY_PY,
	RTW_COUNTRY_QA,
	RTW_COUNTRY_RS,
	RTW_COUNTRY_RU,
	RTW_COUNTRY_RW,
	RTW_COUNTRY_SA,
	RTW_COUNTRY_SE,
	RTW_COUNTRY_SG,
	RTW_COUNTRY_SI,
	RTW_COUNTRY_SK,
	RTW_COUNTRY_SN,
	RTW_COUNTRY_SR,
	RTW_COUNTRY_SV,
	RTW_COUNTRY_SY,
	RTW_COUNTRY_TC,
	RTW_COUNTRY_TD,
	RTW_COUNTRY_TG,
	RTW_COUNTRY_TH,
	RTW_COUNTRY_TN,
	RTW_COUNTRY_TR,
	RTW_COUNTRY_TT,
	RTW_COUNTRY_TZ,
	RTW_COUNTRY_UA,
	RTW_COUNTRY_UG,
	RTW_COUNTRY_UY,
	RTW_COUNTRY_UZ,
	RTW_COUNTRY_VC,
	RTW_COUNTRY_VE,
	RTW_COUNTRY_VN,
	RTW_COUNTRY_VU,
	RTW_COUNTRY_WF,
	RTW_COUNTRY_WS,
	RTW_COUNTRY_YE,
	RTW_COUNTRY_YT,
	RTW_COUNTRY_ZA,
	RTW_COUNTRY_ZW,
	
	RTW_COUNTRY_MAX

}rtw_country_code_t;


typedef enum {
	RTW_MODE_NONE = 0,
	RTW_MODE_STA,
	RTW_MODE_AP,
	RTW_MODE_STA_AP,
	RTW_MODE_PROMISC,
	RTW_MODE_P2P
}rtw_mode_t;

typedef enum {
	RTW_SCAN_FULL = 0,
	RTW_SCAN_SOCIAL,
	RTW_SCAN_ONE
}rtw_scan_mode_t;

typedef enum {
	RTW_LINK_DISCONNECTED = 0,
	RTW_LINK_CONNECTED
} rtw_link_status_t;

typedef enum {
    RTW_SCAN_TYPE_ACTIVE              = 0x00,  /**< Actively scan a network by sending 802.11 probe(s)         */
    RTW_SCAN_TYPE_PASSIVE             = 0x01,  /**< Passively scan a network by listening for beacons from APs */
    RTW_SCAN_TYPE_PROHIBITED_CHANNELS = 0x04   /**< Passively scan on channels not enabled by the country code */
} rtw_scan_type_t;

typedef enum {
    RTW_BSS_TYPE_INFRASTRUCTURE = 0, /**< Denotes infrastructure network                  */
    RTW_BSS_TYPE_ADHOC          = 1, /**< Denotes an 802.11 ad-hoc IBSS network           */
    RTW_BSS_TYPE_ANY            = 2, /**< Denotes either infrastructure or ad-hoc network */

    RTW_BSS_TYPE_UNKNOWN        = -1 /**< May be returned by scan function if BSS type is unknown. Do not pass this to the Join function */
} rtw_bss_type_t;

typedef enum {
	RTW_SCAN_COMMAMD = 0x01
} rtw_scan_command_t;

typedef enum{
	COMMAND1					= 0x01
}rtw_command_type;

typedef enum {
	RTW_WPS_TYPE_DEFAULT 		    	= 0x0000,
	RTW_WPS_TYPE_USER_SPECIFIED 		= 0x0001,
	RTW_WPS_TYPE_MACHINE_SPECIFIED   	= 0x0002,
	RTW_WPS_TYPE_REKEY 			        = 0x0003,
	RTW_WPS_TYPE_PUSHBUTTON 		    = 0x0004,
	RTW_WPS_TYPE_REGISTRAR_SPECIFIED 	= 0x0005,
    RTW_WPS_TYPE_NONE                   = 0x0006 
} rtw_wps_type_t;

typedef enum {
    RTW_NETWORK_B   = 1,
	RTW_NETWORK_BG  = 3,
	RTW_NETWORK_BGN = 11
} rtw_network_mode_t;

typedef enum {
    RTW_STA_INTERFACE     = 0, /**< STA or Client Interface  */
    RTW_AP_INTERFACE      = 1, /**< softAP Interface         */
} rtw_interface_t;

/**
 * Enumeration of packet filter rules
 */
typedef enum {
	RTW_POSITIVE_MATCHING  = 0, /**< Specifies that a filter should match a given pattern     */
	RTW_NEGATIVE_MATCHING  = 1  /**< Specifies that a filter should NOT match a given pattern */
} rtw_packet_filter_rule_e;

typedef enum {
	RTW_PROMISC_DISABLE = 0,  /**< disable the promisc */
	RTW_PROMISC_ENABLE = 1,   /**< fetch all ethernet packets */
	RTW_PROMISC_ENABLE_1 = 2, /**< fetch only B/M packets */
	RTW_PROMISC_ENABLE_2 = 3, /**< fetch all 802.11 packets*/
	RTW_PROMISC_ENABLE_3 = 4, /**< fetch only B/M 802.11 packets*/
} rtw_rcr_level_t;

typedef enum{
	RTW_NO_ERROR = 0,
	RTW_NONE_NETWORK = 1,
	RTW_CONNECT_FAIL = 2,
	RTW_WRONG_PASSWORD = 3 ,
	RTW_DHCP_FAIL = 4,
	RTW_UNKNOWN,
}rtw_connect_error_flag_t;

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
#ifdef	__cplusplus
}
#endif
#endif /* _WIFI_CONSTANTS_H */
