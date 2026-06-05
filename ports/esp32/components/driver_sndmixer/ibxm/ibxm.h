
/* ibxm/ac mod/xm/s3m replay (c)mumart@gmail.com */

//Set to 1 to save the ASCII info (instruments, songname, ...) when loading
#define IBXM_SAVE_ASCII_INFO 0
//Set to 1 to output mono samples
#define IBXM_MONO 1

extern const char *IBXM_VERSION;

struct data {
	char *buffer;
	int length;
};

//Native, allocated samples are always 0 flags.
//Samples as used directly from input memory have one of these flags.
#define SAMPLE_8BIT (1<<0)
#define SAMPLE_UNSIGNED (1<<1)
#define SAMPLE_DELTA (1<<2)
#define SAMPLE_DONTFREE (1<<3)
#define SAMPLE_PINGPONG (1<<4)

struct delta_cache {
	int pos;
	int amp;
};

struct sample {
#if IBXM_SAVE_ASCII_INFO
	char name[ 32 ];
#endif
	int loop_start, loop_length;
	short volume, panning, rel_note, fine_tune;
	short *data;
	int flags;
	struct delta_cache *dcache;
};

struct envelope {
	char enabled, sustain, looped, num_points;
	short sustain_tick, loop_start_tick, loop_end_tick;
	short points_tick[ 16 ], points_ampl[ 16 ];
};

struct instrument {
	int num_samples, vol_fadeout;
#if IBXM_SAVE_ASCII_INFO
	char name[ 32 ];
#endif
	char *key_to_sample;
	char vib_type, vib_sweep, vib_depth, vib_rate;
	struct envelope vol_env, pan_env;
	struct sample *samples;
};

struct pattern {
	int num_channels, num_rows;
	char *data;
	int data_idx;
};

typedef struct module module;
typedef void(*pattern_cache_hdl_t)(module *module, int idx);

struct module {
	struct data data;
#if IBXM_SAVE_ASCII_INFO
	char name[ 32 ];
#endif
	int num_channels, num_instruments;
	int num_patterns, sequence_len, restart_pos;
	int default_gvol, default_speed, default_tempo, c2_rate, gain;
	int linear_periods, fast_vol_slides;
	unsigned char *default_panning, *sequence;
	struct pattern *patterns;
	struct instrument *instruments;
	struct pattern pattern_cache;
	int pattern_cache_idx;
	pattern_cache_hdl_t pattern_cache_handler;
};

/* Allocate and initialize a module from the specified data, returns NULL on error.
   Message should point to a 64-character buffer to receive error messages. */
struct module* module_load( struct data *data, char *message );
/* Deallocate the specified module. */
void dispose_module( struct module *module );
/* Allocate and initialize a replay with the specified module and sampling rate. */
struct replay* new_replay( struct module *module, int sample_rate, int interpolation );
/* Deallocate the specified replay. */
void dispose_replay( struct replay *replay );
/* Returns the song duration in samples at the current sampling rate. */
int replay_calculate_duration( struct replay *replay );
/* Seek to approximately the specified sample position.
   The actual sample position reached is returned. */
int replay_seek( struct replay *replay, int sample_pos );
/* Set the pattern in the sequence to play. The tempo is reset to the default. */
void replay_set_sequence_pos( struct replay *replay, int pos );
/* Generates audio and returns the number of stereo samples written into mix_buf. */
int replay_get_audio( struct replay *replay, int *mix_buf );
/* Returns the length of the output buffer required by replay_get_audio(). */
int calculate_mix_buf_len( int sample_rate );
