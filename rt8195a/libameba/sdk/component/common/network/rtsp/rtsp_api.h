#ifndef _RTSP_H_
#define _RTSP_H_

#include "dlist.h"
#include "osdep_api.h"
#include "avcodec.h"

#define RTSP_DEBUG 0

#if RTSP_DEBUG
#define RTSP_PRINTF(fmt, args...)    printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define RTSP_ERROR(fmt, args...)    printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#else
#define RTSP_PRINTF(fmt, args...)
#define RTSP_ERROR(fmt, args...)    printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#endif


/*******************************************************general***************************************************************************/

struct stream_context
{	
	struct list_head payload_entry;
	_Mutex entry_lock;
	_Sema sema;
	void *protoCtx; // tranport layer protocol info.(eg.RTSP/HTTP)
	int isProcess;
        int allowStream;
        int pt; //payload type
        struct codec_info codec;
        void *priv; // pointer to private data
};

/************************************************************** start of rtsp *************************************************************/

/*rtsp command list*/
#define CMD_OPTIONS 1
#define CMD_DESCRIBE 2
#define CMD_SETUP 3
#define CMD_TEARDOWN 4
#define CMD_PLAY 5
#define CMD_PAUSE 6

/*rtsp cast mode list*/
#define UNICAST_UDP_MODE 1
#define UNICAST_TCP_MODE 2
#define MULTICAST_MODE 3

#define CRLF "\r\n"
#define BOUNDARY "amebaimagetest"
#define RTSP_RESPONSE_SIZE 	512 //max size for request/response buffer  
#define SERVER_PORT_RANGE_MIN 55608
#define SERVER_PORT_RANGE_MAX 55625
#define PORT_RANGE_MIN 50020
#define PORT_RANGE_MAX 50036

#if (SERVER_PORT_RANGE_MIN >= SERVER_PORT_RANGE_MAX) || (PORT_RANGE_MIN >= PORT_RANGE_MAX)
 #error "port range invalid"
#endif
#if ((SERVER_PORT_RANGE_MIN%2) != 0) || ((PORT_RANGE_MIN%2) != 0)
 #error "minimal port value should be even"
#endif

#define DEF_RTSP_PORT 554
#define DEF_HTTP_PORT 5008

/************************************************************** end of rtsp *************************************************************/

/************************************************************** start of rtp **************************************************************/

#define DEF_RTP_PORT 27750
#define RTP_BIG_ENDIAN 0
#define RTP_HDR_SZ 12
/*rtp payload type mapping and standard rtp payload type table*/
#define RTP_PT_PCMU     0
#define RTP_PT_GSM      3
#define RTP_PT_G723     4
#define RTP_PT_DVI4_R8000        5
#define RTP_PT_DVI4_R16000       6
#define RTP_PT_LPC      7
#define RTP_PT_PCMA     8
#define RTP_PT_G722     9
#define RTP_PT_L16_C2   10
#define RTP_PT_L16_C1   11
#define RTP_PT_QCELP    12
#define RTP_PT_CN       13
#define RTP_PT_MPA      14
#define RTP_PT_G728     15
#define RTP_PT_DVI4_R11025      16
#define RTP_PT_DVI4_R22050      17
#define RTP_PT_G719     18
#define RTP_PT_CELB     25
#define RTP_PT_JPEG     26
#define RTP_PT_NV       28
#define RTP_PT_H261     31
#define RTP_PT_MPV      32
#define RTP_PT_MP2T     33
#define RTP_PT_H263     34
#define RTP_PT_RTCP_BASE        72
#define RTP_PT_DYN_BASE         96
#define RTP_PT_UNKNOWN          -1

#include "section_config.h"
SDRAM_DATA_SECTION
static const struct {
     int pt; //payload type
     const char enc_name[6];
     int clock_rate;
     int audio_channels;
 } rtp_payload_types[] = {
    //{0, "PCMU", 8000, 1},
    //{3, "GSM", 8000, 1},
    //{4, "G723", 8000, 1},
    //{5, "DVI4", 8000, 1},
    //{6, "DVI4", 16000, 1},
    //{7, "LPC", 8000, 1},
    //{8, "PCMA", 8000, 1},
    //{9, "G722", 8000, 1},
    //{10, "L16", 44100, 2},
    //{11, "L16", 44100, 1},
    //{12, "QCELP", 8000, 1},
    //{13, "CN", 8000, 1},
    //{14, "MPA", -1, -1},
    //{15, "G728", 8000, 1},
    //{16, "DVI4", 11025, 1},
    //{17, "DVI4", 22050, 1},
    //{18, "G729", 8000, 1},
    //{25, "CelB", 90000, -1},
    {26, "JPEG", 90000, -1},
    //{28, "nv", 90000, -1},
    //{31, "H261", 90000, -1},
    //{32, "MPV", 90000, -1},
    //{33, "MP2T", 90000, -1},
    //{34, "H263", 90000, -1},
    {-1, "", -1, -1}
};

#define RTP_STD_PT_SIZE  /*25*/ 2      //length of rtp_payload_types 

/************************************************************** end of rtp **************************************************************/

/************************************************************** start of rtcp *************************************************************/
#define RTP_SEQ_MOD (1<<16)
#define RTP_MAX_SDES 255      /* maximum text length for SDES */

typedef enum {
  RTCP_SR   = 200,
  RTCP_RR   = 201,
  RTCP_SDES = 202,
  RTCP_BYE  = 203,
  RTCP_APP  = 204
} rtcp_type_t;

typedef enum {
  RTCP_SDES_END   = 0,
  RTCP_SDES_CNAME = 1,
  RTCP_SDES_NAME  = 2,
  RTCP_SDES_EMAIL = 3,
  RTCP_SDES_PHONE = 4,
  RTCP_SDES_LOC   = 5,
  RTCP_SDES_TOOL  = 6,
  RTCP_SDES_NOTE  = 7,
  RTCP_SDES_PRIV  = 8
} rtcp_sdes_type_t;

/************************************************************** end of rtcp *************************************************************/

/************************************************************** start of rtsp *************************************************************/

typedef enum _rtsp_state rtsp_state;
enum _rtsp_state {
    RTSP_INIT = 0,
    RTSP_READY = 1,
    RTSP_PLAYING = 2,
};

struct rtsp_session
{
        int id;
        int version;
        int start_time;
        int end_time;
        unsigned char *user;
        unsigned char *name;
};

struct rtsp_transport
{
	int isRtp; //transport protocol
	int isTcp; //lower transport protocol
	int castMode; //unicast UDP(1) or unicast TCP(2) or multicast(3)
	int port_min;
	int port_max;
	int clientport_min;
	int clientport_max;
	int serverport_min;
	int serverport_max;
	int ttl; //multicast time to live
	//to be added if necessary
};

/*sturcture to hold connect info*/
struct connect_context
{
	int socket_id;
	u16 port;
	unsigned char *server_ip;
        unsigned char *remote_ip;
};


struct rtsp_context
{
	rtsp_state state; //state for rtp streaming
	int rtsp_cmd; //store request method type
	int Cseq;
	struct rtsp_transport transport;
        struct rtsp_session session;
	u8 *response; //enough to hold response
	int content_len;
	struct connect_context connect_ctx; //contain rtsp server info.
	void *stream_ctx; //stream context it process
	_Sema start_rtp_sema; //sema to start rtp processing
        int is_rtp_start;
          //to be added if necessary
};

/************************************************************** end of rtsp *************************************************************/

/************************************************************** start of rtp **************************************************************/


/*
 * RTP data header from RFC1889
 */
/*
 *
 *
 *    The RTP header has the following format:
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P|X|  CC   |M|     PT      |       sequence number         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           timestamp                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |           synchronization source (SSRC) identifier            |
 * +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 * |            contributing source (CSRC) identifiers             |
 * |                             ....                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * RTP data header
 */

typedef struct {
#if RTP_BIG_ENDIAN
        unsigned int version:2;   /* protocol version */
        unsigned int p:1;         /* padding flag */
        unsigned int x:1;         /* header extension flag */
        unsigned int cc:4;        /* CSRC count */
        unsigned int m:1;         /* marker bit */
        unsigned int pt:7;        /* payload type */
#else /*RTP_LITTLE_ENDIAN*/
        unsigned int cc:4;        /* CSRC count */
        unsigned int x:1;         /* header extension flag */
        unsigned int p:1;         /* padding flag */
        unsigned int version:2;   /* protocol version */
        unsigned int pt:7;        /* payload type */
        unsigned int m:1;         /* marker bit */
#endif
        u16 seq;              /* sequence number */
        u32 ts;               /* timestamp */
        u32 ssrc;             /* synchronization source */
//        u32 csrc[1];          /* optional CSRC list, skip because cc is set to 0 here*/
} rtp_hdr_t;

enum rtp_object_state
{
    RTP_OBJECT_IDLE = 1,
    RTP_OBJECT_READY,
    RTP_OBJECT_INUSE,
    RTP_OBJECT_USED,
};

struct rtp_object
{
        struct list_head rtp_list;
        _Mutex mutex;
        rtp_hdr_t rtphdr;
        u32 *extra;     //pointer to type specific structure
        int index;      
        unsigned char *data;
        int len;
        enum rtp_object_state state;    //reserved for future use
        struct connect_context connect_ctx;
};

/************************************************************** end of rtp **************************************************************/

/***************************************************************general******************************************************************/
void stream_context_init(struct stream_context *stream_ctx);
void stream_context_free(struct stream_context *stream_ctx);

/************************************************************** start of rtsp *************************************************************/

int rtsp_atoi(char * s);
int rtsp_context_init(struct rtsp_context *rtsp_ctx);
void rtsp_readheader(u8 *request_header);
u8 * rtsp_parsefield(u8 *buffer, char *field); //parse specific field line and return field data
u8 * rtsp_readline(struct rtsp_context *rtsp_ctx, u8 *ptr); //read line starting from ptr and store corresponding parameter in rtsp_ctx 
void rtsp_getparam(struct rtsp_context *rtsp_ctx, u8 *rtsp_header);
void parse_rtsp_type(struct rtsp_context *rtsp_ctx, u8 *rtsp_header);
int rtp_tick_inc_to_fps(int tick_inc, int clock_rate);
int rtp_fps_to_tick_inc(int framerate, int clock_rate);
void gen_session_id(int *session_id);
void rtsp_formSdp(struct rtsp_context *rtsp_ctx, u8 *sdp_buffer, int size);
void rtsp_cmd_options(struct rtsp_context *rtsp_ctx);
void rtsp_cmd_describe(struct rtsp_context *rtsp_ctx);
void rtsp_cmd_setup(struct rtsp_context *rtsp_ctx);
void rtsp_cmd_play(struct rtsp_context *rtsp_ctx);
void rtsp_cmd_pause(struct rtsp_context *rtsp_ctx);
void rtsp_cmd_teardown(struct rtsp_context *rtsp_ctx);
void rtsp_cmd_error(struct rtsp_context *rtsp_ctx);
void rtsp_init(void *param);

/************************************************************** end of rtsp *************************************************************/

/************************************************************** start of rtp **************************************************************/

void fillRtpHeader(rtp_hdr_t *rtphdr, int version, int padding, int extension, int cc, int marker, int pt, u16 seq, u32 ts, u32 ssrc);

/************************************************************** end of rtp **************************************************************/

#endif