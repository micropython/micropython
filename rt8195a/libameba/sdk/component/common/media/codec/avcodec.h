
#ifndef _AV_CODEC_H_
#define _AV_CODEC_H_

struct codec_info
{
       int codec_id;
       const char codec_name[8];
       int clock_rate;
       int audio_channels;
       int bit_rate;
       void *priv;
};

/* media type list*/
#define AVMEDIA_TYPE_VIDEO 1
#define AVMEDIA_TYPE_AUDIO 2
#define AVMEDIA_TYPE_SUBTITLE  3
#define AVMEDIA_TYPE_UNKNOWN -1

/*codec id list*/

#define AV_CODEC_ID_MJPEG 1
#define AV_CODEC_ID_H264  2
#define AV_CODEC_ID_UNKNOWN -1

/*av codec tables*/
#include "section_config.h"
SDRAM_DATA_SECTION
static const struct codec_info av_codec_tables[] = {
  {AV_CODEC_ID_MJPEG, "MJPEG", 90000, -1, 0, 0},
  {AV_CODEC_ID_H264, "H264", 90000, -1, 0, 0},
  {AV_CODEC_ID_UNKNOWN, "", 0, -1, 0, 0}
};
#define AVCODEC_SIZE    3

void get_codec_by_id(struct codec_info *c, int id);

#endif  //_AV_CODEC_H_