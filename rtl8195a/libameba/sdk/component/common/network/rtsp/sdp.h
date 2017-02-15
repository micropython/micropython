
#ifndef _SDP_H_
#define _SDP_H_

/* select one codec profile, here we choose AV PROFILE*/
#include "avcodec.h"

#define MAX_SDP_SIZE 512
#define SDP_LINE_LEN 128


struct sdp_session_level {
     int sdp_version;      /**< protocol version (currently 0) */
     int id;               /**< session ID */
     int version;          /**< session version */
     int start_time;       /**< session start time (NTP time, in seconds), or 0 in case of permanent session */
     int end_time;         /**< session end time (NTP time, in seconds), or 0 if the session is not bounded */
     int ttl;              /**< TTL, in case of multicast stream */
     const char *user;     /**< username of the session's creator */
     const char *nettype;  /**< type of network (initially "IN") */
     const char *src_addr; /**< IP address of the machine from which the session was created */
     const char *src_type; /**< address type of src_addr */
     const char *dst_addr; /**< destination IP address (can be multicast) */
     const char *dst_type; /**< destination IP address type */
     const char *name;     /**< session name (can be an empty string) */
};

struct sdp_media_level {
     int media_type;
     int payload_type;
     int framerate;
     struct codec_info *codec;
};

struct sdp_info {
     struct sdp_session_level session;
     struct sdp_media_level media;
     void *extra;
};


int form_sdp_header(unsigned char *buffer, struct sdp_info *sdp, int size);
int form_sdp_media(unsigned char *buffer, struct sdp_info *sdp, int size);

#endif  //_SDP_H_