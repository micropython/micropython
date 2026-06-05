
#include "stdlib.h"
#include "string.h"
#include <stdio.h>

#include "ibxm.h"

#if 1
#define DO_CALLOC calloc
#else
#define DO_CALLOC(n, s) my_calloc(n, s, __FUNCTION__, __LINE__)

void *my_calloc(size_t nmemb, size_t size, const char *function, int line) {
	static int tot=0;
	tot+=size*nmemb;
//	printf("%s:%d: Allocating %d items of %d (total %d K)\n", function, line, nmemb, size, (nmemb*size)/1024);
	printf("%d K (%d x %d) %s:%d (tot %dK)\n", (nmemb*size)/1024, nmemb, size, function, line, tot/1024);
	return calloc(nmemb, size);
}
#endif

#if IBXM_MONO
#define BYTES_PER_SAMPLE 2
#else
#define BYTES_PER_SAMPLE 4
#endif

const char *IBXM_VERSION = "ibxm/ac mod/xm/s3m replay 20171013 (c)mumart@gmail.com";

static const int FP_SHIFT = 15, FP_ONE = 32768, FP_MASK = 32767;

static const int exp2_table[] = {
	32768, 32946, 33125, 33305, 33486, 33667, 33850, 34034,
	34219, 34405, 34591, 34779, 34968, 35158, 35349, 35541,
	35734, 35928, 36123, 36319, 36516, 36715, 36914, 37114,
	37316, 37518, 37722, 37927, 38133, 38340, 38548, 38757,
	38968, 39180, 39392, 39606, 39821, 40037, 40255, 40473,
	40693, 40914, 41136, 41360, 41584, 41810, 42037, 42265,
	42495, 42726, 42958, 43191, 43425, 43661, 43898, 44137,
	44376, 44617, 44859, 45103, 45348, 45594, 45842, 46091,
	46341, 46593, 46846, 47100, 47356, 47613, 47871, 48131,
	48393, 48655, 48920, 49185, 49452, 49721, 49991, 50262,
	50535, 50810, 51085, 51363, 51642, 51922, 52204, 52488,
	52773, 53059, 53347, 53637, 53928, 54221, 54515, 54811,
	55109, 55408, 55709, 56012, 56316, 56622, 56929, 57238,
	57549, 57861, 58176, 58491, 58809, 59128, 59449, 59772,
	60097, 60423, 60751, 61081, 61413, 61746, 62081, 62419,
	62757, 63098, 63441, 63785, 64132, 64480, 64830, 65182,
	65536
};

static const short sine_table[] = {
	  0,  24,  49,  74,  97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
	255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120,  97,  74,  49,  24
};

struct note {
	unsigned char key, instrument, volume, effect, param;
};

struct channel {
	struct replay *replay;
	struct instrument *instrument;
	struct sample *sample;
	struct note note;
	int id, key_on, random_seed, pl_row;
	int sample_off, sample_idx, sample_fra, freq, ampl, pann;
	int volume, panning, fadeout_vol, vol_env_tick, pan_env_tick;
	int period, porta_period, retrig_count, fx_count, av_count;
	int porta_up_param, porta_down_param, tone_porta_param, offset_param;
	int fine_porta_up_param, fine_porta_down_param, xfine_porta_param;
	int arpeggio_param, vol_slide_param, gvol_slide_param, pan_slide_param;
	int fine_vslide_up_param, fine_vslide_down_param;
	int retrig_volume, retrig_ticks, tremor_on_ticks, tremor_off_ticks;
	int vibrato_type, vibrato_phase, vibrato_speed, vibrato_depth;
	int tremolo_type, tremolo_phase, tremolo_speed, tremolo_depth;
	int tremolo_add, vibrato_add, arpeggio_add;
};

struct replay {
	int sample_rate, interpolation, global_vol;
	int seq_pos, break_pos, row, next_row, tick;
	int speed, tempo, pl_count, pl_chan;
	int *ramp_buf;
	char **play_count;
	struct channel *channels;
	struct module *module;
};

static int exp_2( int x ) {
	int c, m, y;
	int x0 = ( x & FP_MASK ) >> ( FP_SHIFT - 7 );
	c = exp2_table[ x0 ];
	m = exp2_table[ x0 + 1 ] - c;
	y = ( m * ( x & ( FP_MASK >> 7 ) ) >> 8 ) + c;
	return ( y << FP_SHIFT ) >> ( FP_SHIFT - ( x >> FP_SHIFT ) );
}

static int log_2( int x ) {
	int step;
	int y = 16 << FP_SHIFT;
	for( step = y; step > 0; step >>= 1 ) {
		if( exp_2( y - step ) >= x ) {
			y -= step;
		}
	}
	return y;
}

static char* data_ascii( struct data *data, int offset, int length, char *dest ) {
	int idx, chr;
	memset( dest, 32, length );
	if( offset > data->length ) {
		offset = data->length;
	}
	if( ( unsigned int ) offset + length > ( unsigned int ) data->length ) {
		length = data->length - offset;
	}
	for( idx = 0; idx < length; idx++ ) {
		chr = data->buffer[ offset + idx ] & 0xFF;
		if( chr > 32 ) {
			dest[ idx ] = chr;
		}
	}
	return dest;
}


static int data_s8( struct data *data, int offset ) {
	int value = 0;
	if( offset < data->length ) {
		value = data->buffer[ offset ];
		value = ( value & 0x7F ) - ( value & 0x80 );
	}
	return value;
}

static int data_u8( struct data *data, int offset ) {
	int value = 0;
	if( offset < data->length ) {
		value = data->buffer[ offset ] & 0xFF;
	}
	return value;
}

static int data_u16be( struct data *data, int offset ) {
	int value = 0;
	if( offset + 1 < data->length ) {
		value = ( ( data->buffer[ offset ] & 0xFF ) << 8 )
			| ( data->buffer[ offset + 1 ] & 0xFF );
	}
	return value;
}

static int data_u16le( struct data *data, int offset ) {
	int value = 0;
	if( offset + 1 < data->length ) {
		value = ( data->buffer[ offset ] & 0xFF )
			| ( ( data->buffer[ offset + 1 ] & 0xFF ) << 8 );
	}
	return value;
}

static unsigned int data_u32le( struct data *data, int offset ) {
	unsigned int value = 0;
	if( offset + 3 < data->length ) {
		value = ( data->buffer[ offset ] & 0xFF )
			| ( ( data->buffer[ offset + 1 ] & 0xFF ) << 8 )
			| ( ( data->buffer[ offset + 2 ] & 0xFF ) << 16 )
			| ( ( data->buffer[ offset + 3 ] & 0xFF ) << 24 );
	}
	return value;
}


static int envelope_next_tick( struct envelope *envelope, int tick, int key_on ) {
	tick++;
	if( envelope->looped && tick >= envelope->loop_end_tick ) {
		tick = envelope->loop_start_tick;
	}
	if( envelope->sustain && key_on && tick >= envelope->sustain_tick ) {
		tick = envelope->sustain_tick;
	}
	return tick;
}
	
static int envelope_calculate_ampl( struct envelope *envelope, int tick ) {
	int idx, point, dt, da;
	int ampl = envelope->points_ampl[ envelope->num_points - 1 ];
	if( tick < envelope->points_tick[ envelope->num_points - 1 ] ) {
		point = 0;
		for( idx = 1; idx < envelope->num_points; idx++ ) {
			if( envelope->points_tick[ idx ] <= tick ) {
				point = idx;
			}
		}
		dt = envelope->points_tick[ point + 1 ] - envelope->points_tick[ point ];
		da = envelope->points_ampl[ point + 1 ] - envelope->points_ampl[ point ];
		ampl = envelope->points_ampl[ point ];
		ampl += ( ( da << 24 ) / dt ) * ( tick - envelope->points_tick[ point ] ) >> 24;
	}
	return ampl;
}

static short get_xm_samp(struct sample *sample, int sidx) {
	int s;
	if (sample->flags & SAMPLE_8BIT) {
		char *bdata=(char*)sample->data;
		s = ( bdata[ sidx ] & 0xFF ) << 8;
		s = ( s & 0x7FFF ) - ( s & 0x8000 );
	} else {
		s = sample->data[sidx]; //WARNING! This assumes a little-endian CPU.
	}
	return s;
}

static short get_sample_data(struct sample *sample, int idx, int channel_no) {
	if (idx==sample->loop_start + sample->loop_length) idx=sample->loop_start;
	short ret;
//	static struct sample *monsample=NULL;
//	if (monsample==NULL) monsample=sample;
//	if (monsample==sample) printf("Sample: %d\n", idx);

	if (sample->flags & SAMPLE_DELTA) {
		idx++; //because the logic afterwards assumes the delta _after_ idx samples. If we didn't do this, idx 0 would always return 0.
		int cache_itm=channel_no+1;

		int amp=sample->dcache[cache_itm].amp;
		int amp_idx=sample->dcache[cache_itm].pos;

		if (idx==0) {
			amp=0;
			amp_idx=0;
		}

		//See if we arrived at the loopback pos
		if (idx==sample->dcache[0].pos) {
			amp=sample->dcache[0].amp;
			amp_idx=sample->dcache[0].pos;
		}

		if (amp_idx>idx) {
			//We need to go back. See if it makes more sense to start over.
			if (amp_idx-idx > idx) {
				amp=0;
				amp_idx=0;
			}
		}

		if (amp_idx<=idx) {
			for(int sidx=amp_idx; sidx < idx; sidx++ ) {
				amp = amp + get_xm_samp(sample, sidx);
//				amp = ( amp & 0x7FFF ) - ( amp & 0x8000 );
			}
		} else {
			for (int sidx=amp_idx-1; sidx >= idx; sidx--) {
				amp = amp - get_xm_samp(sample, sidx);
//				amp = ( amp & 0x7FFF ) - ( amp & 0x8000 );
			}
		}


		if (idx==sample->loop_start && sample->dcache[0].pos==0) {
			//We're at the loop pos. Save those stats.
			sample->dcache[0].pos=idx;
			sample->dcache[0].amp=amp;
		}
		sample->dcache[cache_itm].pos=idx;
		sample->dcache[cache_itm].amp=amp;
		return amp;
	}

	if (sample->flags & SAMPLE_8BIT) {
		unsigned char *bdata=(unsigned char*)sample->data;
		ret=bdata[idx]*256;
	} else {
		ret=sample->data[idx];
	}
	if (sample->flags & SAMPLE_UNSIGNED) {
		int v=(unsigned short)ret;
		ret=v-32768;
	}

	if (sample->flags & SAMPLE_PINGPONG) {
		printf("Ping-pong unsupported\n");
	}

	return ret;
}

static struct pattern *get_pattern(struct module *module, int idx) {
	if (module->pattern_cache_handler) {
		if (module->pattern_cache_idx != idx) {
			module->pattern_cache_handler(module, idx);
			module->pattern_cache_idx = idx;
		}
		return &module->pattern_cache;
	} else {
		return &module->patterns[idx];
	}
}

/* Deallocate the specified module. */
void dispose_module( struct module *module ) {
	int idx, sam;
	struct instrument *instrument;
	free( module->default_panning );
	free( module->sequence );
	free( module->pattern_cache.data );
	if( module->patterns ) {
		for( idx = 0; idx < module->num_patterns; idx++ ) {
			free( module->patterns[ idx ].data );
		}
		free( module->patterns );
	}
	if( module->instruments ) {
		for( idx = 0; idx <= module->num_instruments; idx++ ) {
			instrument = &module->instruments[ idx ];
			if( instrument->samples ) {
				for( sam = 0; sam < instrument->num_samples; sam++ ) {
					if (!(instrument->samples[sam].flags & SAMPLE_DONTFREE)) {
						 free( instrument->samples[ sam ].data );
					}
					free( instrument->samples[ sam ].dcache );
				}
				free( instrument->samples );
			}
		}
		free( module->instruments );
	}
	free( module );
}

static void module_cache_handler_xm(struct module *module, int idx) {
	int key, ins, vol, fxc, fxp, flags;
	int note, num_notes;
	char *pattern_data = module->pattern_cache.data;
	module->pattern_cache.num_channels=module->patterns[idx].num_channels;
	module->pattern_cache.num_rows=module->patterns[idx].num_rows;
	struct data *data=&module->data;
	int offset=module->patterns[idx].data_idx;
	num_notes = module->patterns[idx].num_rows * module->num_channels;
	memset(pattern_data, 0, num_notes* 5);
	int pat_data_offset=0;
	for( note = 0; note < num_notes; note++ ) {
		flags = data_u8( data, offset );
		if( ( flags & 0x80 ) == 0 ) {
			flags = 0x1F;
		} else {
			offset++;
		}
		key = ( flags & 0x01 ) > 0 ? data_u8( data, offset++ ) : 0;
		pattern_data[ pat_data_offset++ ] = key;
		ins = ( flags & 0x02 ) > 0 ? data_u8( data, offset++ ) : 0;
		pattern_data[ pat_data_offset++ ] = ins;
		vol = ( flags & 0x04 ) > 0 ? data_u8( data, offset++ ) : 0;
		pattern_data[ pat_data_offset++ ] = vol;
		fxc = ( flags & 0x08 ) > 0 ? data_u8( data, offset++ ) : 0;
		fxp = ( flags & 0x10 ) > 0 ? data_u8( data, offset++ ) : 0;
		if( fxc >= 0x40 ) {
			fxc = fxp = 0;
		}
		pattern_data[ pat_data_offset++ ] = fxc;
		pattern_data[ pat_data_offset++ ] = fxp;
	}
}

static struct module* module_load_xm( struct data *data, char *message ) {
	int delta_env, offset, next_offset, idx, entry;
	int num_rows, pat_data_len;
	int sam, sam_head_offset, sam_data_bytes, sam_data_samples;
	int num_samples, sam_loop_start, sam_loop_length;
	int ins;
	int point, point_tick, point_offset;
	int looped, ping_pong, sixteen_bit;
	char ascii[ 16 ];
	struct instrument *instrument;
	struct sample *sample;
	struct module *module = DO_CALLOC( 1, sizeof( struct module ) );
	if( module ) {
		if( data_u16le( data, 58 ) != 0x0104 ) {
			strcpy( message, "XM format version must be 0x0104!" );
			dispose_module( module );
			return NULL;
		}
		memcpy(&module->data, data, sizeof(struct data));
#if IBXM_SAVE_ASCII_INFO
		data_ascii( data, 17, 20, module->name );
#endif
		delta_env = !memcmp( data_ascii( data, 38, 15, ascii ), "DigiBooster Pro", 15 );
		offset = 60 + data_u32le( data, 60 );
		module->sequence_len = data_u16le( data, 64 );
		module->restart_pos = data_u16le( data, 66 );
		module->num_channels = data_u16le( data, 68 );
		module->num_patterns = data_u16le( data, 70 );
		module->num_instruments = data_u16le( data, 72 );
		module->linear_periods = data_u16le( data, 74 ) & 0x1;
		module->default_gvol = 64;
		module->default_speed = data_u16le( data, 76 );
		module->default_tempo = data_u16le( data, 78 );
		module->c2_rate = 8363;
		module->gain = 64;
		module->default_panning = DO_CALLOC( module->num_channels, sizeof( unsigned char ) );
		module->pattern_cache_handler=module_cache_handler_xm;
		module->pattern_cache_idx=-1;
		if( !module->default_panning ) {
			dispose_module( module );
			return NULL;
		}
		for( idx = 0; idx < module->num_channels; idx++ ) {
			module->default_panning[ idx ] = 128;
		}
		module->sequence = DO_CALLOC( module->sequence_len, sizeof( unsigned char ) );
		if( !module->sequence ) {
			dispose_module( module );
			return NULL;
		}
		for( idx = 0; idx < module->sequence_len; idx++ ) {
			entry = data_u8( data, 80 + idx );
			module->sequence[ idx ] = entry < module->num_patterns ? entry : 0;
		}
		module->patterns = DO_CALLOC( module->num_patterns, sizeof( struct pattern ) );
		if( !module->patterns ) {
			dispose_module( module );
			return NULL;
		}
		//Can't allocate pattern cache data yet; we need to size it to contain the largest pattern...
		int max_rows=0;
		for( idx = 0; idx < module->num_patterns; idx++ ) {
			if( data_u8( data, offset + 4 ) ) {
				strcpy( message, "Unknown pattern packing type!" );
				dispose_module( module );
				return NULL;
			}
			num_rows = data_u16le( data, offset + 5 );
			if( num_rows < 1 ) {
				num_rows = 1;
			}
			if (num_rows>max_rows) max_rows=num_rows;

			pat_data_len = data_u16le( data, offset + 7 );
			offset += data_u32le( data, offset );
			next_offset = offset + pat_data_len;
			module->patterns[ idx ].num_channels = module->num_channels;
			module->patterns[ idx ].num_rows = num_rows;
			module->patterns[ idx ].data_idx = offset;
			offset = next_offset;
		}
		module->pattern_cache.data = DO_CALLOC(module->num_channels * max_rows, 5);
		module->instruments = DO_CALLOC( module->num_instruments + 1, sizeof( struct instrument ) );
		if( !module->instruments ) {
			dispose_module( module );
			return NULL;
		}
		instrument = &module->instruments[ 0 ];
		instrument->samples = DO_CALLOC( 1, sizeof( struct sample ) );
		if( !instrument->samples ) {
			dispose_module( module );
			return NULL;
		}
		for( ins = 1; ins <= module->num_instruments; ins++ ) {
			instrument = &module->instruments[ ins ];
#if IBXM_SAVE_ASCII_INFO
			data_ascii( data, offset + 4, 22, instrument->name );
#endif
			num_samples = data_u16le( data, offset + 27 );
			instrument->num_samples = ( num_samples > 0 ) ? num_samples : 1;
			instrument->samples = DO_CALLOC( instrument->num_samples, sizeof( struct sample ) );
			if( !instrument->samples ) {
				dispose_module( module );
				return NULL;
			}
			if( num_samples > 0 ) {
				instrument->key_to_sample = (char*)data->buffer+(offset + 33 -1);
				point_tick = 0;
				for( point = 0; point < 12; point++ ) {
					point_offset = offset + 129 + ( point * 4 );
					point_tick = ( delta_env ? point_tick : 0 ) + data_u16le( data, point_offset );
					instrument->vol_env.points_tick[ point ] = point_tick;
					instrument->vol_env.points_ampl[ point ] = data_u16le( data, point_offset + 2 );
				}
				point_tick = 0;
				for( point = 0; point < 12; point++ ) {
					point_offset = offset + 177 + ( point * 4 );
					point_tick = ( delta_env ? point_tick : 0 ) + data_u16le( data, point_offset );
					instrument->pan_env.points_tick[ point ] = point_tick;
					instrument->pan_env.points_ampl[ point ] = data_u16le( data, point_offset + 2 );
				}
				instrument->vol_env.num_points = data_u8( data, offset + 225 );
				if( instrument->vol_env.num_points > 12 ) {
					instrument->vol_env.num_points = 0;
				}
				instrument->pan_env.num_points = data_u8( data, offset + 226 );
				if( instrument->pan_env.num_points > 12 ) {
					instrument->pan_env.num_points = 0;
				}
				instrument->vol_env.sustain_tick = instrument->vol_env.points_tick[ data_u8( data, offset + 227 ) & 0xF ];
				instrument->vol_env.loop_start_tick = instrument->vol_env.points_tick[ data_u8( data, offset + 228 ) & 0xF ];
				instrument->vol_env.loop_end_tick = instrument->vol_env.points_tick[ data_u8( data, offset + 229 ) & 0xF ];
				instrument->pan_env.sustain_tick = instrument->pan_env.points_tick[ data_u8( data, offset + 230 ) & 0xF ];
				instrument->pan_env.loop_start_tick = instrument->pan_env.points_tick[ data_u8( data, offset + 231 ) & 0xF ];
				instrument->pan_env.loop_end_tick = instrument->pan_env.points_tick[ data_u8( data, offset + 232 ) & 0xF ];
				instrument->vol_env.enabled = instrument->vol_env.num_points > 0 && ( data_u8( data, offset + 233 ) & 0x1 );
				instrument->vol_env.sustain = ( data_u8( data, offset + 233 ) & 0x2 ) > 0;
				instrument->vol_env.looped = ( data_u8( data, offset + 233 ) & 0x4 ) > 0;
				instrument->pan_env.enabled = instrument->pan_env.num_points > 0 && ( data_u8( data, offset + 234 ) & 0x1 );
				instrument->pan_env.sustain = ( data_u8( data, offset + 234 ) & 0x2 ) > 0;
				instrument->pan_env.looped = ( data_u8( data, offset + 234 ) & 0x4 ) > 0;
				instrument->vib_type = data_u8( data, offset + 235 );
				instrument->vib_sweep = data_u8( data, offset + 236 );
				instrument->vib_depth = data_u8( data, offset + 237 );
				instrument->vib_rate = data_u8( data, offset + 238 );
				instrument->vol_fadeout = data_u16le( data, offset + 239 );
			}
			offset += data_u32le( data, offset );
			sam_head_offset = offset;
			offset += num_samples * 40;
			for( sam = 0; sam < num_samples; sam++ ) {
				sample = &instrument->samples[ sam ];
				sam_data_bytes = data_u32le( data, sam_head_offset );
				sam_loop_start = data_u32le( data, sam_head_offset + 4 );
				sam_loop_length = data_u32le( data, sam_head_offset + 8 );
				sample->volume = data_u8( data, sam_head_offset + 12 );
				sample->fine_tune = data_s8( data, sam_head_offset + 13 );
				looped = ( data_u8( data, sam_head_offset + 14 ) & 0x3 ) > 0;
				ping_pong = ( data_u8( data, sam_head_offset + 14 ) & 0x2 ) > 0;
				sixteen_bit = ( data_u8( data, sam_head_offset + 14 ) & 0x10 ) > 0;
				sample->panning = data_u8( data, sam_head_offset + 15 ) + 1;
				sample->rel_note = data_s8( data, sam_head_offset + 16 );
#if IBXM_SAVE_ASCII_INFO
				data_ascii( data, sam_head_offset + 18, 22, sample->name );
#endif
				sam_head_offset += 40;
				sam_data_samples = sam_data_bytes;
				if( sixteen_bit ) {
					sam_data_samples = sam_data_samples >> 1;
					sam_loop_start = sam_loop_start >> 1;
					sam_loop_length = sam_loop_length >> 1;
				}
				if( !looped || ( sam_loop_start + sam_loop_length ) > sam_data_samples ) {
					sam_loop_start = sam_data_samples;
					sam_loop_length = 0;
				}
				sample->dcache=DO_CALLOC(module->num_channels+1, sizeof(struct delta_cache));
				if (!sample->dcache) {
					dispose_module( module );
					return NULL;
				}
				sample->loop_start = sam_loop_start;
				sample->loop_length = sam_loop_length;
				sample->flags = SAMPLE_DONTFREE | SAMPLE_DELTA;
				if (!sixteen_bit) sample->flags|=SAMPLE_8BIT;
				if (ping_pong) sample->flags|=SAMPLE_PINGPONG;
				sample->data=(short*)&data->buffer[offset];
				offset += sam_data_bytes;
			}
		}
	}
	return module;
}

static void module_cache_handler_s3m(struct module *module, int idx) {
	int key, ins, volume, effect, param;
	int note_offset, row, chan, token;
	int channel_map[ 32 ];
	char *pattern_data = module->pattern_cache.data;
	module->pattern_cache.num_channels=module->patterns[idx].num_channels;
	module->pattern_cache.num_rows=module->patterns[idx].num_rows;
	struct data *data=&module->data;
	int pat_offset=module->patterns[idx].data_idx;
	memset(pattern_data, 0, module->num_channels * 64* 5);

	int nc=0;
	for( int nidx = 0; nidx < 32; nidx++ ) {
		channel_map[ nidx ] = -1;
		if( data_u8( data, 64 + nidx ) < 16 ) {
			channel_map[ nidx ] = nc++;
		}
	}

	row = 0;
	while( row < 64 ) {
		token = data_u8( data, pat_offset++ );
		if( token ) {
			key = ins = 0;
			if( ( token & 0x20 ) == 0x20 ) {
				/* Key + Instrument.*/
				key = data_u8( data, pat_offset++ );
				ins = data_u8( data, pat_offset++ );
				if( key < 0xFE ) {
					key = ( key >> 4 ) * 12 + ( key & 0xF ) + 1;
				} else if( key == 0xFF ) {
					key = 0;
				}
			}
			volume = 0;
			if( ( token & 0x40 ) == 0x40 ) {
				/* Volume Column.*/
				volume = ( data_u8( data, pat_offset++ ) & 0x7F ) + 0x10;
				if( volume > 0x50 ) {
					volume = 0;
				}
			}
			effect = param = 0;
			if( ( token & 0x80 ) == 0x80 ) {
				/* Effect + Param.*/
				effect = data_u8( data, pat_offset++ );
				param = data_u8( data, pat_offset++ );
				if( effect < 1 || effect >= 0x40 ) {
					effect = param = 0;
				} else if( effect > 0 ) {
					effect += 0x80;
				}
			}
			chan = channel_map[ token & 0x1F ];
			if( chan >= 0 ) {
				note_offset = ( row * module->num_channels + chan ) * 5;
				pattern_data[ note_offset     ] = key;
				pattern_data[ note_offset + 1 ] = ins;
				pattern_data[ note_offset + 2 ] = volume;
				pattern_data[ note_offset + 3 ] = effect;
				pattern_data[ note_offset + 4 ] = param;
			}
		} else {
			row++;
		}
	}
}

static struct module* module_load_s3m( struct data *data, char *message ) {
	int idx, module_data_idx, inst_offset, flags;
	int version, sixteen_bit, tune, signed_samples;
	int stereo_mode, default_pan, panning, ins, chan;
	int sample_offset, sample_length, loop_start, loop_length;
	int channel_map[ 32 ];
	struct instrument *instrument;
	struct sample *sample;
	struct module *module = DO_CALLOC( 1, sizeof( struct module ) );
	if( module ) {
		memcpy(&module->data, data, sizeof(struct data));
#if IBXM_SAVE_ASCII_INFO
		data_ascii( data, 0, 28, module->name );
#endif
		module->sequence_len = data_u16le( data, 32 );
		module->num_instruments = data_u16le( data, 34 );
		module->num_patterns = data_u16le( data, 36 );
		module->pattern_cache_handler=module_cache_handler_s3m;
		module->pattern_cache_idx=-1;
		flags = data_u16le( data, 38 );
		version = data_u16le( data, 40 );
		module->fast_vol_slides = ( ( flags & 0x40 ) == 0x40 ) || version == 0x1300;
		signed_samples = data_u16le( data, 42 ) == 1;
		if( data_u32le( data, 44 ) != 0x4d524353 ) {
			strcpy( message, "Not an S3M file!" );
			dispose_module( module );
			return NULL;
		}
		module->default_gvol = data_u8( data, 48 );
		module->default_speed = data_u8( data, 49 );
		module->default_tempo = data_u8( data, 50 );
		module->c2_rate = 8363;
		module->gain = data_u8( data, 51 ) & 0x7F;
		stereo_mode = ( data_u8( data, 51 ) & 0x80 ) == 0x80;
		default_pan = data_u8( data, 53 ) == 0xFC;
		for( idx = 0; idx < 32; idx++ ) {
			channel_map[ idx ] = -1;
			if( data_u8( data, 64 + idx ) < 16 ) {
				channel_map[ idx ] = module->num_channels++;
			}
		}
		module->sequence = DO_CALLOC( module->sequence_len, sizeof( unsigned char ) );
		if( !module->sequence ){
			dispose_module( module );
			return NULL;
		}
		for( idx = 0; idx < module->sequence_len; idx++ ) {
			module->sequence[ idx ] = data_u8( data, 96 + idx );
		}
		module_data_idx = 96 + module->sequence_len;
		module->instruments = DO_CALLOC( module->num_instruments + 1, sizeof( struct instrument ) );
		if( !module->instruments ) {
			dispose_module( module );
			return NULL;
		}
		instrument = &module->instruments[ 0 ];
		instrument->num_samples = 1;
		instrument->samples = DO_CALLOC( 1, sizeof( struct sample ) );
		if( !instrument->samples ) {
			dispose_module( module );
			return NULL;
		}
		for( ins = 1; ins <= module->num_instruments; ins++ ) {
			instrument = &module->instruments[ ins ];
			instrument->num_samples = 1;
			instrument->samples = DO_CALLOC( 1, sizeof( struct sample ) );
			if( !instrument->samples ) {
				dispose_module( module );
				return NULL;
			}
			sample = &instrument->samples[ 0 ];
			inst_offset = data_u16le( data, module_data_idx ) << 4;
			module_data_idx += 2;
#if IBXM_SAVE_ASCII_INFO
			data_ascii( data, inst_offset + 48, 28, instrument->name );
#endif
			if( data_u8( data, inst_offset ) == 1 && data_u16le( data, inst_offset + 76 ) == 0x4353 ) {
				sample_offset = ( data_u8( data, inst_offset + 13 ) << 20 )
					+ ( data_u16le( data, inst_offset + 14 ) << 4 );
				sample_length = data_u32le( data, inst_offset + 16 );
				loop_start = data_u32le( data, inst_offset + 20 );
				loop_length = data_u32le( data, inst_offset + 24 ) - loop_start;
				sample->volume = data_u8( data, inst_offset + 28 );
				if( data_u8( data, inst_offset + 30 ) != 0 ) {
					strcpy( message, "Packed samples not supported!" );
					dispose_module( module );
					return NULL;
				}
				if( loop_start + loop_length > sample_length ) {
					loop_length = sample_length - loop_start;
				}
				if( loop_length < 1 || !( data_u8( data, inst_offset + 31 ) & 0x1 ) ) {
					loop_start = sample_length;
					loop_length = 0;
				}
				sample->loop_start = loop_start;
				sample->loop_length = loop_length;
				/* stereo = data_u8( data, inst_offset + 31 ) & 0x2; */
				sixteen_bit = data_u8( data, inst_offset + 31 ) & 0x4;
				tune = ( log_2( data_u32le( data, inst_offset + 32 ) ) - log_2( module->c2_rate ) ) * 12;
				sample->rel_note = tune >> FP_SHIFT;
				sample->fine_tune = ( tune & FP_MASK ) >> ( FP_SHIFT - 7 );
				sample->flags=SAMPLE_DONTFREE;
				sample->data=(short*)&data->buffer[sample_offset];
				if( !sixteen_bit ) {
					sample->flags|=SAMPLE_8BIT;
				}
				if( !signed_samples ) {
					sample->flags|=SAMPLE_UNSIGNED;
				}
			}
		}
		module->patterns = DO_CALLOC( module->num_patterns, sizeof( struct pattern ) );
		module->pattern_cache.data = DO_CALLOC(module->num_channels * 64, 5);
		if( !module->patterns || !module->pattern_cache.data) {
			dispose_module( module );
			return NULL;
		}
		for( idx = 0; idx < module->num_patterns; idx++ ) {
			module->patterns[ idx ].num_channels = module->num_channels;
			module->patterns[ idx ].num_rows = 64;
			module->patterns[ idx ].data_idx = ( data_u16le( data, module_data_idx ) << 4 ) + 2;
			module_data_idx += 2;
		}
		module->default_panning = DO_CALLOC( module->num_channels, sizeof( unsigned char ) );
		if( module->default_panning ) {
			for( chan = 0; chan < 32; chan++ ) {
				if( channel_map[ chan ] >= 0 ) {
					panning = 7;
					if( stereo_mode ) {
						panning = 12;
						if( data_u8( data, 64 + chan ) < 8 ) {
							panning = 3;
						}
					}
					if( default_pan ) {
						flags = data_u8( data, module_data_idx + chan );
						if( ( flags & 0x20 ) == 0x20 ) {
							panning = flags & 0xF;
						}
					}
					module->default_panning[ channel_map[ chan ] ] = panning * 17;
				}
			}
		} else {
			dispose_module( module );
			return NULL;
		}
	}
	return module;
}

static void module_cache_handler_mod(struct module *module, int idx) {
	int key, ins, effect, param, period;
	int pat_data_len = module->num_channels * 64 * 5;
	char *pattern_data = module->pattern_cache.data;
	module->pattern_cache.num_channels=module->patterns[idx].num_channels;
	module->pattern_cache.num_rows=module->patterns[idx].num_rows;
	struct data *data=&module->data;
	int module_data_idx=module->patterns[idx].data_idx;
	memset(pattern_data, 0, module->num_channels * 64* 5);
	for( int pat_data_idx = 0; pat_data_idx < pat_data_len; pat_data_idx += 5 ) {
		period = ( data_u8( data, module_data_idx ) & 0xF ) << 8;
		period = ( period | data_u8( data, module_data_idx + 1 ) ) * 4;
		if( period >= 112 && period <= 6848 ) {
			key = -12 * log_2( ( period << FP_SHIFT ) / 29021 );
			key = ( key + ( key & ( FP_ONE >> 1 ) ) ) >> FP_SHIFT;
			pattern_data[ pat_data_idx ] = key;
		}
		ins = ( data_u8( data, module_data_idx + 2 ) & 0xF0 ) >> 4;
		ins = ins | ( data_u8( data, module_data_idx ) & 0x10 );
		pattern_data[ pat_data_idx + 1 ] = ins;
		effect = data_u8( data, module_data_idx + 2 ) & 0x0F;
		param  = data_u8( data, module_data_idx + 3 );
		if( param == 0 && ( effect < 3 || effect == 0xA ) ) {
			effect = 0;
		}
		if( param == 0 && ( effect == 5 || effect == 6 ) ) {
			effect -= 2;
		}
		if( effect == 8 && module->num_channels == 4 ) {
			effect = param = 0;
		}
		pattern_data[ pat_data_idx + 3 ] = effect;
		pattern_data[ pat_data_idx + 4 ] = param;
		module_data_idx+=4;
	}
}

static struct module* module_load_mod( struct data *data, char *message ) {
	int idx, pat, module_data_idx, pat_data_len;
	int ins, fine_tune;
	int sample_length, loop_start, loop_length;
	struct instrument *instrument;
	struct sample *sample;
	struct module *module = DO_CALLOC( 1, sizeof( struct module ) );
	if( module ) {
		memcpy(&module->data, data, sizeof(struct data));
#if IBXM_SAVE_ASCII_INFO
		data_ascii( data, 0, 20, module->name );
#endif
		module->sequence_len = data_u8( data, 950 ) & 0x7F;
		module->restart_pos = data_u8( data, 951 ) & 0x7F;
		module->pattern_cache_handler=module_cache_handler_mod;
		module->pattern_cache_idx=-1;
		if( module->restart_pos >= module->sequence_len ) {
			module->restart_pos = 0;
		}
		module->sequence = DO_CALLOC( 128, sizeof( unsigned char ) );
		if( !module->sequence ){
			dispose_module( module );
			return NULL;
		}
		for( idx = 0; idx < 128; idx++ ) {
			pat = data_u8( data, 952 + idx ) & 0x7F;
			module->sequence[ idx ] = pat;
			if( pat >= module->num_patterns ) {
				module->num_patterns = pat + 1;
			}
		}
		switch( data_u16be( data, 1082 ) ) {
			case 0x4b2e: /* M.K. */
			case 0x4b21: /* M!K! */
			case 0x5434: /* FLT4 */
				module->num_channels = 4;
				module->c2_rate = 8287;
				module->gain = 64;
				break;
			case 0x484e: /* xCHN */
				module->num_channels = data_u8( data, 1080 ) - 48;
				module->c2_rate = 8363;
				module->gain = 32;
				break;
			case 0x4348: /* xxCH */
				module->num_channels = ( data_u8( data, 1080 ) - 48 ) * 10;
				module->num_channels += data_u8( data, 1081 ) - 48;
				module->c2_rate = 8363;
				module->gain = 32;
				break;
			default:
				strcpy( message, "MOD Format not recognised!" );
				dispose_module( module );
				return NULL;
		}
		module->default_gvol = 64;
		module->default_speed = 6;
		module->default_tempo = 125;
		module->default_panning = DO_CALLOC( module->num_channels, sizeof( unsigned char ) );
		if( !module->default_panning ) {
			dispose_module( module );
			return NULL;
		}
		for( idx = 0; idx < module->num_channels; idx++ ) {
			module->default_panning[ idx ] = 51;
			if( ( idx & 3 ) == 1 || ( idx & 3 ) == 2 ) {
				module->default_panning[ idx ] = 204;
			}
		}
		module_data_idx = 1084;
		module->patterns = DO_CALLOC( module->num_patterns, sizeof( struct pattern ) );
		pat_data_len = module->num_channels * 64 * 5;
		module->pattern_cache.data = DO_CALLOC( 1, pat_data_len );
		if( !module->patterns || !module->pattern_cache.data ) {
			dispose_module( module );
			return NULL;
		}
		for( pat = 0; pat < module->num_patterns; pat++ ) {
			module->patterns[ pat ].num_channels = module->num_channels;
			module->patterns[ pat ].num_rows = 64;
			module->patterns[ pat ].data_idx = module_data_idx;
			module_data_idx += module->num_channels * 64 * 4;
		}
		module->num_instruments = 31;
		module->instruments = DO_CALLOC( module->num_instruments + 1, sizeof( struct instrument ) );
		if( !module->instruments ) {
			dispose_module( module );
			return NULL;
		}
		instrument = &module->instruments[ 0 ];
		instrument->num_samples = 1;
		instrument->samples = DO_CALLOC( 1, sizeof( struct sample ) );
		if( !instrument->samples ) {
			dispose_module( module );
			return NULL;
		}
		for( ins = 1; ins <= module->num_instruments; ins++ ) {
			instrument = &module->instruments[ ins ];
			instrument->num_samples = 1;
			instrument->samples = DO_CALLOC( 1, sizeof( struct sample ) );
			if( !instrument->samples ) {
				dispose_module( module );
				return NULL;
			}
			sample = &instrument->samples[ 0 ];
#if IBXM_SAVE_ASCII_INFO
			data_ascii( data, ins * 30 - 10, 22, instrument->name );
#endif
			sample_length = data_u16be( data, ins * 30 + 12 ) * 2;
			fine_tune = ( data_u8( data, ins * 30 + 14 ) & 0xF ) << 4;
			sample->fine_tune = ( fine_tune & 0x7F ) - ( fine_tune & 0x80 );
			sample->volume = data_u8( data, ins * 30 + 15 ) & 0x7F;
			if( sample->volume > 64 ) {
				sample->volume = 64;
			}
			loop_start = data_u16be( data, ins * 30 + 16 ) * 2;
			loop_length = data_u16be( data, ins * 30 + 18 ) * 2;
			if( loop_start + loop_length > sample_length ) {
				if( loop_start / 2 + loop_length <= sample_length ) {
					/* Some old modules have loop start in bytes. */
					loop_start = loop_start / 2;
				} else {
					loop_length = sample_length - loop_start;
				}
			}
			if( loop_length < 4 ) {
				loop_start = sample_length;
				loop_length = 0;
			}
			sample->loop_start = loop_start;
			sample->loop_length = loop_length;
			sample->data = (short*)&data->buffer[module_data_idx];
			sample->flags=SAMPLE_DONTFREE|SAMPLE_8BIT;
			module_data_idx += sample_length;
		}
	}
	return module;
}

/* Allocate and initialize a module from the specified data, returns NULL on error.
   Message should point to a 64-character buffer to receive error messages. */
struct module* module_load( struct data *data, char *message ) {
	char ascii[ 16 ];
	struct module* module;
	if( !memcmp( data_ascii( data, 0, 16, ascii ), "Extended Module:", 16 ) ) {
		module = module_load_xm( data, message );
	} else if( !memcmp( data_ascii( data, 44, 4, ascii ), "SCRM", 4 ) ) {
		module = module_load_s3m( data, message );
	} else {
		module = module_load_mod( data, message );
	}
	return module;
}

static void pattern_get_note( struct pattern *pattern, int row, int chan, struct note *dest ) {
	int offset = ( row * pattern->num_channels + chan ) * 5;
	if( offset >= 0 && row < pattern->num_rows && chan < pattern->num_channels ) {
		dest->key = pattern->data[ offset ];
		dest->instrument = pattern->data[ offset + 1 ];
		dest->volume = pattern->data[ offset + 2 ];
		dest->effect = pattern->data[ offset + 3 ];
		dest->param = pattern->data[ offset + 4 ];
	} else {
		memset( dest, 0, sizeof( struct note ) );
	}
}

static void channel_init( struct channel *channel, struct replay *replay, int idx ) {
	memset( channel, 0, sizeof( struct channel ) );
	channel->replay = replay;
	channel->id = idx;
	channel->panning = replay->module->default_panning[ idx ];
	channel->instrument = &replay->module->instruments[ 0 ];
	channel->sample = &channel->instrument->samples[ 0 ];
	channel->random_seed = ( idx + 1 ) * 0xABCDEF;
}

static void channel_volume_slide( struct channel *channel ) {
	int up = channel->vol_slide_param >> 4;
	int down = channel->vol_slide_param & 0xF;
	if( down == 0xF && up > 0 ) {
		/* Fine slide up.*/
		if( channel->fx_count == 0 ) {
			channel->volume += up;
		}
	} else if( up == 0xF && down > 0 ) {
		/* Fine slide down.*/
		if( channel->fx_count == 0 ) {
			channel->volume -= down;
		}
	} else if( channel->fx_count > 0 || channel->replay->module->fast_vol_slides ) {
		/* Normal.*/
		channel->volume += up - down;
	}
	if( channel->volume > 64 ) {
		channel->volume = 64;
	}
	if( channel->volume < 0 ) {
		channel->volume = 0;
	}
}

static void channel_porta_up( struct channel *channel, int param ) {
	switch( param & 0xF0 ) {
		case 0xE0: /* Extra-fine porta.*/
			if( channel->fx_count == 0 ) {
				channel->period -= param & 0xF;
			}
			break;
		case 0xF0: /* Fine porta.*/
			if( channel->fx_count == 0 ) {
				channel->period -= ( param & 0xF ) << 2;
			}
			break;
		default:/* Normal porta.*/
			if( channel->fx_count > 0 ) {
				channel->period -= param << 2;
			}
			break;
	}
	if( channel->period < 0 ) {
		channel->period = 0;
	}
}

static void channel_porta_down( struct channel *channel, int param ) {
	if( channel->period > 0 ) {
		switch( param & 0xF0 ) {
			case 0xE0: /* Extra-fine porta.*/
				if( channel->fx_count == 0 ) {
					channel->period += param & 0xF;
				}
				break;
			case 0xF0: /* Fine porta.*/
				if( channel->fx_count == 0 ) {
					channel->period += ( param & 0xF ) << 2;
				}
				break;
			default:/* Normal porta.*/
				if( channel->fx_count > 0 ) {
					channel->period += param << 2;
				}
				break;
		}
		if( channel->period > 65535 ) {
			channel->period = 65535;
		}
	}
}

static void channel_tone_porta( struct channel *channel ) {
	if( channel->period > 0 ) {
		if( channel->period < channel->porta_period ) {
			channel->period += channel->tone_porta_param << 2;
			if( channel->period > channel->porta_period ) {
				channel->period = channel->porta_period;
			}
		} else {
			channel->period -= channel->tone_porta_param << 2;
			if( channel->period < channel->porta_period ) {
				channel->period = channel->porta_period;
			}
		}
	}
}

static int channel_waveform( struct channel *channel, int phase, int type ) {
	int amplitude = 0;
	switch( type ) {
		default: /* Sine. */
			amplitude = sine_table[ phase & 0x1F ];
			if( ( phase & 0x20 ) > 0 ) {
				amplitude = -amplitude;
			}
			break;
		case 6: /* Saw Up.*/
			amplitude = ( ( ( phase + 0x20 ) & 0x3F ) << 3 ) - 255;
			break;
		case 1: case 7: /* Saw Down. */
			amplitude = 255 - ( ( ( phase + 0x20 ) & 0x3F ) << 3 );
			break;
		case 2: case 5: /* Square. */
			amplitude = ( phase & 0x20 ) > 0 ? 255 : -255;
			break;
		case 3: case 8: /* Random. */
			amplitude = ( channel->random_seed >> 20 ) - 255;
			channel->random_seed = ( channel->random_seed * 65 + 17 ) & 0x1FFFFFFF;
			break;
	}
	return amplitude;
}

static void channel_vibrato( struct channel *channel, int fine ) {
	int wave = channel_waveform( channel, channel->vibrato_phase, channel->vibrato_type & 0x3 );
	channel->vibrato_add = wave * channel->vibrato_depth >> ( fine ? 7 : 5 );
}

static void channel_tremolo( struct channel *channel ) {
	int wave = channel_waveform( channel, channel->tremolo_phase, channel->tremolo_type & 0x3 );
	channel->tremolo_add = wave * channel->tremolo_depth >> 6;
}

static void channel_tremor( struct channel *channel ) {
	if( channel->retrig_count >= channel->tremor_on_ticks ) {
		channel->tremolo_add = -64;
	}
	if( channel->retrig_count >= ( channel->tremor_on_ticks + channel->tremor_off_ticks ) ) {
		channel->tremolo_add = channel->retrig_count = 0;
	}
}

static void channel_retrig_vol_slide( struct channel *channel ) {
	if( channel->retrig_count >= channel->retrig_ticks ) {
		channel->retrig_count = channel->sample_idx = channel->sample_fra = 0;
		switch( channel->retrig_volume ) {
			case 0x1: channel->volume = channel->volume -  1; break;
			case 0x2: channel->volume = channel->volume -  2; break;
			case 0x3: channel->volume = channel->volume -  4; break;
			case 0x4: channel->volume = channel->volume -  8; break;
			case 0x5: channel->volume = channel->volume - 16; break;
			case 0x6: channel->volume = channel->volume * 2 / 3; break;
			case 0x7: channel->volume = channel->volume >> 1; break;
			case 0x8: /* ? */ break;
			case 0x9: channel->volume = channel->volume +  1; break;
			case 0xA: channel->volume = channel->volume +  2; break;
			case 0xB: channel->volume = channel->volume +  4; break;
			case 0xC: channel->volume = channel->volume +  8; break;
			case 0xD: channel->volume = channel->volume + 16; break;
			case 0xE: channel->volume = channel->volume * 3 / 2; break;
			case 0xF: channel->volume = channel->volume << 1; break;
		}
		if( channel->volume <  0 ) {
			channel->volume = 0;
		}
		if( channel->volume > 64 ) {
			channel->volume = 64;
		}
	}
}

static void channel_trigger( struct channel *channel ) {
	int key, sam, porta, period, fine_tune, ins = channel->note.instrument;
	struct sample *sample;
	if( ins > 0 && ins <= channel->replay->module->num_instruments ) {
		channel->instrument = &channel->replay->module->instruments[ ins ];
		key = channel->note.key < 97 ? channel->note.key : 0;
		sam = channel->instrument->key_to_sample ? channel->instrument->key_to_sample[ key ] : 0;
		sample = &channel->instrument->samples[ sam ];
		channel->volume = sample->volume >= 64 ? 64 : sample->volume & 0x3F;
		if( sample->panning > 0 ) {
			channel->panning = ( sample->panning - 1 ) & 0xFF;
		}
		if( channel->period > 0 && sample->loop_length > 1 ) {
			/* Amiga trigger.*/
			channel->sample = sample;
		}
		channel->sample_off = 0;
		channel->vol_env_tick = channel->pan_env_tick = 0;
		channel->fadeout_vol = 32768;
		channel->key_on = 1;
	}
	if( channel->note.effect == 0x09 || channel->note.effect == 0x8F ) {
		/* Set Sample Offset. */
		if( channel->note.param > 0 ) {
			channel->offset_param = channel->note.param;
		}
		channel->sample_off = channel->offset_param << 8;
	}
	if( channel->note.volume >= 0x10 && channel->note.volume < 0x60 ) {
		channel->volume = channel->note.volume < 0x50 ? channel->note.volume - 0x10 : 64;
	}
	switch( channel->note.volume & 0xF0 ) {
		case 0x80: /* Fine Vol Down.*/
			channel->volume -= channel->note.volume & 0xF;
			if( channel->volume < 0 ) {
				channel->volume = 0;
			}
			break;
		case 0x90: /* Fine Vol Up.*/
			channel->volume += channel->note.volume & 0xF;
			if( channel->volume > 64 ) {
				channel->volume = 64;
			}
			break;
		case 0xA0: /* Set Vibrato Speed.*/
			if( ( channel->note.volume & 0xF ) > 0 ) {
				channel->vibrato_speed = channel->note.volume & 0xF;
			}
			break;
		case 0xB0: /* Vibrato.*/
			if( ( channel->note.volume & 0xF ) > 0 ) {
				channel->vibrato_depth = channel->note.volume & 0xF;
			}
			channel_vibrato( channel, 0 );
			break;
		case 0xC0: /* Set Panning.*/
			channel->panning = ( channel->note.volume & 0xF ) * 17;
			break;
		case 0xF0: /* Tone Porta.*/
			if( ( channel->note.volume & 0xF ) > 0 ) {
				channel->tone_porta_param = channel->note.volume & 0xF;
			}
			break;
	}
	if( channel->note.key > 0 ) {
		if( channel->note.key > 96 ) {
			channel->key_on = 0;
		} else {
			porta = ( channel->note.volume & 0xF0 ) == 0xF0 ||
				channel->note.effect == 0x03 || channel->note.effect == 0x05 ||
				channel->note.effect == 0x87 || channel->note.effect == 0x8C;
			if( !porta ) {
				ins = channel->instrument->key_to_sample ? channel->instrument->key_to_sample[ channel->note.key ] : 0;
				channel->sample = &channel->instrument->samples[ ins ];
			}
			fine_tune = channel->sample->fine_tune;
			if( channel->note.effect == 0x75 || channel->note.effect == 0xF2 ) {
				/* Set Fine Tune. */
				fine_tune = ( ( channel->note.param & 0xF ) << 4 ) - 128;
			}
			key = channel->note.key + channel->sample->rel_note;
			if( key < 1 ) {
				key = 1;
			}
			if( key > 120 ) {
				key = 120;
			}
			period = ( key << 6 ) + ( fine_tune >> 1 );
			if( channel->replay->module->linear_periods ) {
				channel->porta_period = 7744 - period;
			} else {
				channel->porta_period = 29021 * exp_2( ( period << FP_SHIFT ) / -768 ) >> FP_SHIFT;
			}
			if( !porta ) {
				channel->period = channel->porta_period;
				channel->sample_idx = channel->sample_off;
				channel->sample_fra = 0;
				if( channel->vibrato_type < 4 ) {
					channel->vibrato_phase = 0;
				}
				if( channel->tremolo_type < 4 ) {
					channel->tremolo_phase = 0;
				}
				channel->retrig_count = channel->av_count = 0;
			}
		}
	}
}

static void channel_update_envelopes( struct channel *channel ) {
	if( channel->instrument->vol_env.enabled ) {
		if( !channel->key_on ) {
			channel->fadeout_vol -= channel->instrument->vol_fadeout;
			if( channel->fadeout_vol < 0 ) {
				channel->fadeout_vol = 0;
			}
		}
		channel->vol_env_tick = envelope_next_tick( &channel->instrument->vol_env,
			channel->vol_env_tick, channel->key_on );
	}
	if( channel->instrument->pan_env.enabled ) {
		channel->pan_env_tick = envelope_next_tick( &channel->instrument->pan_env,
			channel->pan_env_tick, channel->key_on );
	}
}

static void channel_auto_vibrato( struct channel *channel ) {
	int sweep, rate, type, wave;
	int depth = channel->instrument->vib_depth & 0x7F;
	if( depth > 0 ) {
		sweep = channel->instrument->vib_sweep & 0x7F;
		rate = channel->instrument->vib_rate & 0x7F;
		type = channel->instrument->vib_type;
		if( channel->av_count < sweep ) {
			depth = depth * channel->av_count / sweep;
		}
		wave = channel_waveform( channel, channel->av_count * rate >> 2, type + 4 );
		channel->vibrato_add += wave * depth >> 8;
		channel->av_count++;
	}
}

static void channel_calculate_freq( struct channel *channel ) {
	int per = channel->period + channel->vibrato_add;
	if( channel->replay->module->linear_periods ) {
		per = per - ( channel->arpeggio_add << 6 );
		if( per < 28 || per > 7680 ) {
			per = 7680;
		}
		channel->freq = ( ( channel->replay->module->c2_rate >> 4 )
			* exp_2( ( ( 4608 - per ) << FP_SHIFT ) / 768 ) ) >> ( FP_SHIFT - 4 );
	} else {
		if( per > 29021 ) {
			per = 29021;
		}
		per = ( per << FP_SHIFT ) / exp_2( ( channel->arpeggio_add << FP_SHIFT ) / 12 );
		if( per < 28 ) {
			per = 29021;
		}
		channel->freq = channel->replay->module->c2_rate * 1712 / per;
	}
}

static void channel_calculate_ampl( struct channel *channel ) {
	int vol, range, env_pan = 32, env_vol = channel->key_on ? 64 : 0;
	if( channel->instrument->vol_env.enabled ) {
		env_vol = envelope_calculate_ampl( &channel->instrument->vol_env, channel->vol_env_tick );
	}
	vol = channel->volume + channel->tremolo_add;
	if( vol > 64 ) {
		vol = 64;
	}
	if( vol < 0 ) {
		vol = 0;
	}
	vol = ( vol * channel->replay->module->gain * FP_ONE ) >> 13;
	vol = ( vol * channel->fadeout_vol ) >> 15;
	channel->ampl = ( vol * channel->replay->global_vol * env_vol ) >> 12;
	if( channel->instrument->pan_env.enabled ) {
		env_pan = envelope_calculate_ampl( &channel->instrument->pan_env, channel->pan_env_tick );
	}
	range = ( channel->panning < 128 ) ? channel->panning : ( 255 - channel->panning );
	channel->pann = channel->panning + ( range * ( env_pan - 32 ) >> 5 );
}

static void channel_tick( struct channel *channel ) {
	channel->vibrato_add = 0;
	channel->fx_count++;
	channel->retrig_count++;
	if( !( channel->note.effect == 0x7D && channel->fx_count <= channel->note.param ) ) {
		switch( channel->note.volume & 0xF0 ) {
			case 0x60: /* Vol Slide Down.*/
				channel->volume -= channel->note.volume & 0xF;
				if( channel->volume < 0 ) {
					channel->volume = 0;
				}
				break;
			case 0x70: /* Vol Slide Up.*/
				channel->volume += channel->note.volume & 0xF;
				if( channel->volume > 64 ) {
					channel->volume = 64;
				}
				break;
			case 0xB0: /* Vibrato.*/
				channel->vibrato_phase += channel->vibrato_speed;
				channel_vibrato( channel, 0 );
				break;
			case 0xD0: /* Pan Slide Left.*/
				channel->panning -= channel->note.volume & 0xF;
				if( channel->panning < 0 ) {
					channel->panning = 0;
				}
				break;
			case 0xE0: /* Pan Slide Right.*/
				channel->panning += channel->note.volume & 0xF;
				if( channel->panning > 255 ) {
					channel->panning = 255;
				}
				break;
			case 0xF0: /* Tone Porta.*/
				channel_tone_porta( channel );
				break;
		}
	}
	switch( channel->note.effect ) {
		case 0x01: case 0x86: /* Porta Up. */
			channel_porta_up( channel, channel->porta_up_param );
			break;
		case 0x02: case 0x85: /* Porta Down. */
			channel_porta_down( channel, channel->porta_down_param );
			break;
		case 0x03: case 0x87: /* Tone Porta. */
			channel_tone_porta( channel );
			break;
		case 0x04: case 0x88: /* Vibrato. */
			channel->vibrato_phase += channel->vibrato_speed;
			channel_vibrato( channel, 0 );
			break;
		case 0x05: case 0x8C: /* Tone Porta + Vol Slide. */
			channel_tone_porta( channel );
			channel_volume_slide( channel );
			break;
		case 0x06: case 0x8B: /* Vibrato + Vol Slide. */
			channel->vibrato_phase += channel->vibrato_speed;
			channel_vibrato( channel, 0 );
			channel_volume_slide( channel );
			break;
		case 0x07: case 0x92: /* Tremolo. */
			channel->tremolo_phase += channel->tremolo_speed;
			channel_tremolo( channel );
			break;
		case 0x0A: case 0x84: /* Vol Slide. */
			channel_volume_slide( channel );
			break;
		case 0x11: /* Global Volume Slide. */
			channel->replay->global_vol = channel->replay->global_vol
				+ ( channel->gvol_slide_param >> 4 )
				- ( channel->gvol_slide_param & 0xF );
			if( channel->replay->global_vol < 0 ) {
				channel->replay->global_vol = 0;
			}
			if( channel->replay->global_vol > 64 ) {
				channel->replay->global_vol = 64;
			}
			break;
		case 0x19: /* Panning Slide. */
			channel->panning = channel->panning
				+ ( channel->pan_slide_param >> 4 )
				- ( channel->pan_slide_param & 0xF );
			if( channel->panning < 0 ) {
				channel->panning = 0;
			}
			if( channel->panning > 255 ) {
				channel->panning = 255;
			}
			break;
		case 0x1B: case 0x91: /* Retrig + Vol Slide. */
			channel_retrig_vol_slide( channel );
			break;
		case 0x1D: case 0x89: /* Tremor. */
			channel_tremor( channel );
			break;
		case 0x79: /* Retrig. */
			if( channel->fx_count >= channel->note.param ) {
				channel->fx_count = 0;
				channel->sample_idx = channel->sample_fra = 0;
			}
			break;
		case 0x7C: case 0xFC: /* Note Cut. */
			if( channel->note.param == channel->fx_count ) {
				channel->volume = 0;
			}
			break;
		case 0x7D: case 0xFD: /* Note Delay. */
			if( channel->note.param == channel->fx_count ) {
				channel_trigger( channel );
			}
			break;
		case 0x8A: /* Arpeggio. */
			if( channel->fx_count == 1 ) {
				channel->arpeggio_add = channel->arpeggio_param >> 4;
			} else if( channel->fx_count == 2 ) {
				channel->arpeggio_add = channel->arpeggio_param & 0xF;
			} else {
				channel->arpeggio_add = channel->fx_count = 0;
			}
			break;
		case 0x95: /* Fine Vibrato. */
			channel->vibrato_phase += channel->vibrato_speed;
			channel_vibrato( channel, 1 );
			break;
	}
	channel_auto_vibrato( channel );
	channel_calculate_freq( channel );
	channel_calculate_ampl( channel );
	channel_update_envelopes( channel );
}

static void channel_row( struct channel *channel, struct note *note ) {
	channel->note = *note;
	channel->retrig_count++;
	channel->vibrato_add = channel->tremolo_add = channel->arpeggio_add = channel->fx_count = 0;
	if( !( ( note->effect == 0x7D || note->effect == 0xFD ) && note->param > 0 ) ) {
		/* Not note delay.*/
		channel_trigger( channel );
	}
	switch( channel->note.effect ) {
		case 0x01: case 0x86: /* Porta Up. */
			if( channel->note.param > 0 ) {
				channel->porta_up_param = channel->note.param;
			}
			channel_porta_up( channel, channel->porta_up_param );
			break;
		case 0x02: case 0x85: /* Porta Down. */
			if( channel->note.param > 0 ) {
				channel->porta_down_param = channel->note.param;
			}
			channel_porta_down( channel, channel->porta_down_param );
			break;
		case 0x03: case 0x87: /* Tone Porta. */
			if( channel->note.param > 0 ) {
				channel->tone_porta_param = channel->note.param;
			}
			break;
		case 0x04: case 0x88: /* Vibrato. */
			if( ( channel->note.param >> 4 ) > 0 ) {
				channel->vibrato_speed = channel->note.param >> 4;
			}
			if( ( channel->note.param & 0xF ) > 0 ) {
				channel->vibrato_depth = channel->note.param & 0xF;
			}
			channel_vibrato( channel, 0 );
			break;
		case 0x05: case 0x8C: /* Tone Porta + Vol Slide. */
			if( channel->note.param > 0 ) {
				channel->vol_slide_param = channel->note.param;
			}
			channel_volume_slide( channel );
			break;
		case 0x06: case 0x8B: /* Vibrato + Vol Slide. */
			if( channel->note.param > 0 ) {
				channel->vol_slide_param = channel->note.param;
			}
			channel_vibrato( channel, 0 );
			channel_volume_slide( channel );
			break;
		case 0x07: case 0x92: /* Tremolo. */
			if( ( channel->note.param >> 4 ) > 0 ) {
				channel->tremolo_speed = channel->note.param >> 4;
			}
			if( ( channel->note.param & 0xF ) > 0 ) {
				channel->tremolo_depth = channel->note.param & 0xF;
			}
			channel_tremolo( channel );
			break;
		case 0x08: /* Set Panning.*/
			channel->panning = ( channel->note.param < 128 ) ? ( channel->note.param << 1 ) : 255;
			break;
		case 0x0A: case 0x84: /* Vol Slide. */
			if( channel->note.param > 0 ) {
				channel->vol_slide_param = channel->note.param;
			}
			channel_volume_slide( channel );
			break;
		case 0x0C: /* Set Volume. */
			channel->volume = channel->note.param >= 64 ? 64 : channel->note.param & 0x3F;
			break;
		case 0x10: case 0x96: /* Set Global Volume. */
			channel->replay->global_vol = channel->note.param >= 64 ? 64 : channel->note.param & 0x3F;
			break;
		case 0x11: /* Global Volume Slide. */
			if( channel->note.param > 0 ) {
				channel->gvol_slide_param = channel->note.param;
			}
			break;
		case 0x14: /* Key Off. */
			channel->key_on = 0;
			break;
		case 0x15: /* Set Envelope Tick. */
			channel->vol_env_tick = channel->pan_env_tick = channel->note.param & 0xFF;
			break;
		case 0x19: /* Panning Slide. */
			if( channel->note.param > 0 ) {
				channel->pan_slide_param = channel->note.param;
			}
			break;
		case 0x1B: case 0x91: /* Retrig + Vol Slide. */
			if( ( channel->note.param >> 4 ) > 0 ) {
				channel->retrig_volume = channel->note.param >> 4;
			}
			if( ( channel->note.param & 0xF ) > 0 ) {
				channel->retrig_ticks = channel->note.param & 0xF;
			}
			channel_retrig_vol_slide( channel );
			break;
		case 0x1D: case 0x89: /* Tremor. */
			if( ( channel->note.param >> 4 ) > 0 ) {
				channel->tremor_on_ticks = channel->note.param >> 4;
			}
			if( ( channel->note.param & 0xF ) > 0 ) {
				channel->tremor_off_ticks = channel->note.param & 0xF;
			}
			channel_tremor( channel );
			break;
		case 0x21: /* Extra Fine Porta. */
			if( channel->note.param > 0 ) {
				channel->xfine_porta_param = channel->note.param;
			}
			switch( channel->xfine_porta_param & 0xF0 ) {
				case 0x10:
					channel_porta_up( channel, 0xE0 | ( channel->xfine_porta_param & 0xF ) );
					break;
				case 0x20:
					channel_porta_down( channel, 0xE0 | ( channel->xfine_porta_param & 0xF ) );
					break;
			}
			break;
		case 0x71: /* Fine Porta Up. */
			if( channel->note.param > 0 ) {
				channel->fine_porta_up_param = channel->note.param;
			}
			channel_porta_up( channel, 0xF0 | ( channel->fine_porta_up_param & 0xF ) );
			break;
		case 0x72: /* Fine Porta Down. */
			if( channel->note.param > 0 ) {
				channel->fine_porta_down_param = channel->note.param;
			}
			channel_porta_down( channel, 0xF0 | ( channel->fine_porta_down_param & 0xF ) );
			break;
		case 0x74: case 0xF3: /* Set Vibrato Waveform. */
			if( channel->note.param < 8 ) {
				channel->vibrato_type = channel->note.param;
			}
			break;
		case 0x77: case 0xF4: /* Set Tremolo Waveform. */
			if( channel->note.param < 8 ) {
				channel->tremolo_type = channel->note.param;
			}
			break;
		case 0x7A: /* Fine Vol Slide Up. */
			if( channel->note.param > 0 ) {
				channel->fine_vslide_up_param = channel->note.param;
			}
			channel->volume += channel->fine_vslide_up_param;
			if( channel->volume > 64 ) {
				channel->volume = 64;
			}
			break;
		case 0x7B: /* Fine Vol Slide Down. */
			if( channel->note.param > 0 ) {
				channel->fine_vslide_down_param = channel->note.param;
			}
			channel->volume -= channel->fine_vslide_down_param;
			if( channel->volume < 0 ) {
				channel->volume = 0;
			}
			break;
		case 0x7C: case 0xFC: /* Note Cut. */
			if( channel->note.param <= 0 ) {
				channel->volume = 0;
			}
			break;
		case 0x8A: /* Arpeggio. */
			if( channel->note.param > 0 ) {
				channel->arpeggio_param = channel->note.param;
			}
			break;
		case 0x95: /* Fine Vibrato.*/
			if( ( channel->note.param >> 4 ) > 0 ) {
				channel->vibrato_speed = channel->note.param >> 4;
			}
			if( ( channel->note.param & 0xF ) > 0 ) {
				channel->vibrato_depth = channel->note.param & 0xF;
			}
			channel_vibrato( channel, 1 );
			break;
		case 0xF8: /* Set Panning. */
			channel->panning = channel->note.param * 17;
			break;
	}
	channel_auto_vibrato( channel );
	channel_calculate_freq( channel );
	channel_calculate_ampl( channel );
	channel_update_envelopes( channel );
}

static void channel_resample( struct channel *channel, int *mix_buf,
		int offset, int count, int sample_rate, int interpolate ) {
	struct sample *sample = channel->sample;
	int sam_idx, sam_fra, step;
	int loop_len, loop_end, out_idx, out_end, y, m, c;
	if( channel->ampl > 0 ) {
#if !IBXM_MONO
		int l_gain, r_gain;
		l_gain = channel->ampl * ( 255 - channel->pann ) >> 8;
		r_gain = channel->ampl * channel->pann >> 8;
#endif
		sam_idx = channel->sample_idx;
		sam_fra = channel->sample_fra;
		step = ( channel->freq << ( FP_SHIFT - 3 ) ) / ( sample_rate >> 3 );
		loop_len = sample->loop_length;
		loop_end = sample->loop_start + loop_len;
		out_idx = offset * (BYTES_PER_SAMPLE/2);
		out_end = ( offset + count ) * (BYTES_PER_SAMPLE/2);
		if( interpolate ) {
			while( out_idx < out_end ) {
				if( sam_idx >= loop_end ) {
					if( loop_len > 1 ) {
						while( sam_idx >= loop_end ) {
							sam_idx -= loop_len;
						}
					} else {
						break;
					}
				}
				c = get_sample_data(channel->sample, sam_idx, channel->id);
				m = get_sample_data(channel->sample, sam_idx + 1, channel->id) - c;
				y = ( ( m * sam_fra ) >> FP_SHIFT ) + c;
#if IBXM_MONO
				mix_buf[ out_idx++ ] += ( y * channel->ampl ) >> FP_SHIFT;
#else
				mix_buf[ out_idx++ ] += ( y * l_gain ) >> FP_SHIFT;
				mix_buf[ out_idx++ ] += ( y * r_gain ) >> FP_SHIFT;
#endif
				sam_fra += step;
				sam_idx += sam_fra >> FP_SHIFT;
				sam_fra &= FP_MASK;
			}
		} else {
			while( out_idx < out_end ) {
				if( sam_idx >= loop_end ) {
					if( loop_len > 1 ) {
						while( sam_idx >= loop_end ) {
							sam_idx -= loop_len;
						}
					} else {
						break;
					}
				}
				y = get_sample_data(channel->sample, sam_idx, channel->id);
#if IBXM_MONO
				mix_buf[ out_idx++ ] += ( y * channel->ampl ) >> FP_SHIFT;
#else
				mix_buf[ out_idx++ ] += ( y * l_gain ) >> FP_SHIFT;
				mix_buf[ out_idx++ ] += ( y * r_gain ) >> FP_SHIFT;
#endif
				sam_fra += step;
				sam_idx += sam_fra >> FP_SHIFT;
				sam_fra &= FP_MASK;
			}
		}
	}
}

static void channel_update_sample_idx( struct channel *channel, int count, int sample_rate ) {
	struct sample *sample = channel->sample;
	int step = ( channel->freq << ( FP_SHIFT - 3 ) ) / ( sample_rate >> 3 );
	channel->sample_fra += step * count;
	channel->sample_idx += channel->sample_fra >> FP_SHIFT;
	if( channel->sample_idx > sample->loop_start ) {
		if( sample->loop_length > 1 ) {
			channel->sample_idx = sample->loop_start
				+ ( channel->sample_idx - sample->loop_start ) % sample->loop_length;
		} else {
			channel->sample_idx = sample->loop_start;
		}
	}
	channel->sample_fra &= FP_MASK;
}

static void replay_row( struct replay *replay ) {
	int idx, count;
	struct note note;
	struct pattern *pattern;
	struct channel *channel;
	struct module *module = replay->module;
	if( replay->next_row < 0 ) {
		replay->break_pos = replay->seq_pos + 1;
		replay->next_row = 0;
	}
	if( replay->break_pos >= 0 ) {
		if( replay->break_pos >= module->sequence_len ) {
			replay->break_pos = replay->next_row = 0;
		}
		while( module->sequence[ replay->break_pos ] >= module->num_patterns ) {
			replay->break_pos++;
			if( replay->break_pos >= module->sequence_len ) {
				replay->break_pos = replay->next_row = 0;
			}
		}
		replay->seq_pos = replay->break_pos;
		for( idx = 0; idx < module->num_channels; idx++ ) {
			replay->channels[ idx ].pl_row = 0;
		}
		replay->break_pos = -1;
	}
	pattern = get_pattern(module, module->sequence[ replay->seq_pos ] );
	replay->row = replay->next_row;
	if( replay->row >= pattern->num_rows ) {
		replay->row = 0;
	}
	if( replay->play_count && replay->play_count[ 0 ] ) {
		count = replay->play_count[ replay->seq_pos ][ replay->row ];
		if( replay->pl_count < 0 && count < 127 ) {
			replay->play_count[ replay->seq_pos ][ replay->row ] = count + 1;
		}
	}
	replay->next_row = replay->row + 1;
	if( replay->next_row >= pattern->num_rows ) {
		replay->next_row = -1;
	}
	for( idx = 0; idx < module->num_channels; idx++ ) {
		channel = &replay->channels[ idx ];
		pattern_get_note( pattern, replay->row, idx, &note );
		if( note.effect == 0xE ) {
			note.effect = 0x70 | ( note.param >> 4 );
			note.param &= 0xF;
		}
		if( note.effect == 0x93 ) {
			note.effect = 0xF0 | ( note.param >> 4 );
			note.param &= 0xF;
		}
		if( note.effect == 0 && note.param > 0 ) {
			note.effect = 0x8A;
		}
		channel_row( channel, &note );
		switch( note.effect ) {
			case 0x81: /* Set Speed. */
				if( note.param > 0 ) {
					replay->tick = replay->speed = note.param;
				}
				break;
			case 0xB: case 0x82: /* Pattern Jump.*/
				if( replay->pl_count < 0 ) {
					replay->break_pos = note.param;
					replay->next_row = 0;
				}
				break;
			case 0xD: case 0x83: /* Pattern Break.*/
				if( replay->pl_count < 0 ) {
					if( replay->break_pos < 0 ) {
						replay->break_pos = replay->seq_pos + 1;
					}
					replay->next_row = ( note.param >> 4 ) * 10 + ( note.param & 0xF );
				}
				break;
			case 0xF: /* Set Speed/Tempo.*/
				if( note.param > 0 ) {
					if( note.param < 32 ) {
						replay->tick = replay->speed = note.param;
					} else {
						replay->tempo = note.param;
					}
				}
				break;
			case 0x94: /* Set Tempo.*/
				if( note.param > 32 ) {
					replay->tempo = note.param;
				}
				break;
			case 0x76: case 0xFB : /* Pattern Loop.*/
				if( note.param == 0 ) {
					/* Set loop marker on this channel. */
					channel->pl_row = replay->row;
				}
				if( channel->pl_row < replay->row && replay->break_pos < 0 ) {
					/* Marker valid. */
					if( replay->pl_count < 0 ) {
						/* Not already looping, begin. */
						replay->pl_count = note.param;
						replay->pl_chan = idx;
					}
					if( replay->pl_chan == idx ) {
						/* Next Loop.*/
						if( replay->pl_count == 0 ) {
							/* Loop finished. Invalidate current marker. */
							channel->pl_row = replay->row + 1;
						} else {
							/* Loop. */
							replay->next_row = channel->pl_row;
						}
						replay->pl_count--;
					}
				}
				break;
			case 0x7E: case 0xFE: /* Pattern Delay.*/
				replay->tick = replay->speed + replay->speed * note.param;
				break;
		}
	}
}

static int replay_tick( struct replay *replay ) {
	int idx, num_channels, count = 1;
	if( --replay->tick <= 0 ) {
		replay->tick = replay->speed;
		replay_row( replay );
	} else {
		num_channels = replay->module->num_channels;
		for( idx = 0; idx < num_channels; idx++ ) {
			channel_tick( &replay->channels[ idx ] );
		}
	}
	if( replay->play_count && replay->play_count[ 0 ] ) {
		count = replay->play_count[ replay->seq_pos ][ replay->row ] - 1;
	}
	return count;
}

static int module_init_play_count( struct module *module, char **play_count ) {
	int idx, pat, rows, len = 0;
	for( idx = 0; idx < module->sequence_len; idx++ ) {
		pat = module->sequence[ idx ];
		if ( pat < module->num_patterns ) {
			struct pattern *patp=get_pattern(module, pat);
			rows=patp->num_rows;
		} else {
			rows=0;
		}
		if( play_count ) {
			play_count[ idx ] = play_count[ 0 ] ? &play_count[ 0 ][ len ] : NULL;
		}
		len += rows;
	}
	return len;
}

/* Set the pattern in the sequence to play. The tempo is reset to the default. */
void replay_set_sequence_pos( struct replay *replay, int pos ) {
	int idx;
	struct module *module = replay->module;
	if( pos >= module->sequence_len ) {
		pos = 0;
	}
	replay->break_pos = pos;
	replay->next_row = 0;
	replay->tick = 1;
	replay->global_vol = module->default_gvol;
	replay->speed = module->default_speed > 0 ? module->default_speed : 6;
	replay->tempo = module->default_tempo > 0 ? module->default_tempo : 125;
	replay->pl_count = replay->pl_chan = -1;
	if( replay->play_count ) {
		free( replay->play_count[ 0 ] );
		free( replay->play_count );
	}
	replay->play_count = DO_CALLOC( module->sequence_len, sizeof( char * ) );
	if( replay->play_count ) {
		replay->play_count[ 0 ] = DO_CALLOC( module_init_play_count( module, NULL ), sizeof( char ) );
		module_init_play_count( module, replay->play_count );
	}
	for( idx = 0; idx < module->num_channels; idx++ ) {
		channel_init( &replay->channels[ idx ], replay, idx );
	}
	memset( replay->ramp_buf, 0, 128 * sizeof( int ) );
	replay_tick( replay );
}

/* Deallocate the specified replay. */
void dispose_replay( struct replay *replay ) {
	if( replay->play_count ) {
		free( replay->play_count[ 0 ] );
		free( replay->play_count );
	}
	free( replay->ramp_buf );
	free( replay->channels );
	free( replay );
}

/* Allocate and initialize a replay with the specified sampling rate and interpolation. */
struct replay* new_replay( struct module *module, int sample_rate, int interpolation ) {
	struct replay *replay = DO_CALLOC( 1, sizeof( struct replay ) );
	if( replay ) {
		replay->module = module;
		replay->sample_rate = sample_rate;
		replay->interpolation = interpolation;
		replay->ramp_buf = DO_CALLOC( 128, sizeof( int ) );
		replay->channels = DO_CALLOC( module->num_channels, sizeof( struct channel ) );
		if( replay->ramp_buf && replay->channels ) {
			replay_set_sequence_pos( replay, 0 );
		} else {
			dispose_replay( replay );
			replay = NULL;
		}
	}
	return replay;
}

static int calculate_tick_len( int tempo, int sample_rate ) {
	return ( sample_rate * 5 ) / ( tempo * 2 );
}

/* Returns the length of the output buffer required by replay_get_audio(). */
int calculate_mix_buf_len( int sample_rate ) {
	return ( calculate_tick_len( 32, sample_rate ) + 65 ) * BYTES_PER_SAMPLE;
}

/* Returns the song duration in samples at the current sampling rate. */
int replay_calculate_duration( struct replay *replay ) {
	int count = 0, duration = 0;
	replay_set_sequence_pos( replay, 0 );
	while( count < 1 ) {
		duration += calculate_tick_len( replay->tempo, replay->sample_rate );
		count = replay_tick( replay );
	}
	replay_set_sequence_pos( replay, 0 );
	return duration;
}

/* Seek to approximately the specified sample position.
   The actual sample position reached is returned. */
int replay_seek( struct replay *replay, int sample_pos ) {
	int idx, tick_len, current_pos = 0;
	replay_set_sequence_pos( replay, 0 );
	tick_len = calculate_tick_len( replay->tempo, replay->sample_rate );
	while( ( sample_pos - current_pos ) >= tick_len ) {
		for( idx = 0; idx < replay->module->num_channels; idx++ ) {
			channel_update_sample_idx( &replay->channels[ idx ],
				tick_len * 2, replay->sample_rate * 2 );
		}
		current_pos += tick_len;
		replay_tick( replay );
		tick_len = calculate_tick_len( replay->tempo, replay->sample_rate );
	}
	return current_pos;
}

static void replay_volume_ramp( struct replay *replay, int *mix_buf, int tick_len ) {
	int idx, a1, a2, ramp_rate = 256 * 2048 / replay->sample_rate;
	for( idx = 0, a1 = 0; a1 < 256; idx += (BYTES_PER_SAMPLE/2), a1 += ramp_rate ) {
		a2 = 256 - a1;
		mix_buf[ idx     ] = ( mix_buf[ idx     ] * a1 + replay->ramp_buf[ idx     ] * a2 ) >> 8;
#if !IBXM_MONO
		mix_buf[ idx + 1 ] = ( mix_buf[ idx + 1 ] * a1 + replay->ramp_buf[ idx + 1 ] * a2 ) >> 8;
#endif
	}
	memcpy( replay->ramp_buf, &mix_buf[ tick_len * (BYTES_PER_SAMPLE/2) ], 128 * sizeof( int ) );
}

/* 2:1 downsampling with simple but effective anti-aliasing. Buf must contain count * 2 + 1 stereo samples. */
static void downsample( int *buf, int count ) {
	int idx, out_idx, out_len = count * 2;
#if !IBXM_MONO
	for( idx = 0, out_idx = 0; out_idx < out_len; idx += 4, out_idx += 2 ) {
		buf[ out_idx     ] = ( buf[ idx     ] >> 2 ) + ( buf[ idx + 2 ] >> 1 ) + ( buf[ idx + 4 ] >> 2 );
		buf[ out_idx + 1 ] = ( buf[ idx + 1 ] >> 2 ) + ( buf[ idx + 3 ] >> 1 ) + ( buf[ idx + 5 ] >> 2 );
	}
#else
	for( idx = 0, out_idx = 0; out_idx < out_len; idx += 2, out_idx += 1 ) {
		buf[ out_idx     ] = ( buf[ idx     ] >> 2 ) + ( buf[ idx + 1 ] >> 1 ) + ( buf[ idx + 2 ] >> 2 );
	}
#endif
}

/* Generates audio and returns the number of stereo samples written into mix_buf. */
int replay_get_audio( struct replay *replay, int *mix_buf ) {
	struct channel *channel;
	int idx, num_channels, tick_len = calculate_tick_len( replay->tempo, replay->sample_rate );
	/* Clear output buffer. */
	memset( mix_buf, 0, ( tick_len + 65 ) * BYTES_PER_SAMPLE * sizeof( int ) );
	/* Resample. */
	num_channels = replay->module->num_channels;
	for( idx = 0; idx < num_channels; idx++ ) {
		channel = &replay->channels[ idx ];
		channel_resample( channel, mix_buf, 0, ( tick_len + 65 ) * 2,
			replay->sample_rate * 2, replay->interpolation );
		channel_update_sample_idx( channel, tick_len * 2, replay->sample_rate * 2 );
	}
	downsample( mix_buf, tick_len + 64 );
	replay_volume_ramp( replay, mix_buf, tick_len );
	replay_tick( replay );
	return tick_len;
}
