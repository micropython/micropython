
#include "errno.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "ibxm.h"
#include <malloc.h>

static long read_file( char *file_name, void *buffer ) {
	long file_length = -1, bytes_read;
	FILE *input_file = fopen( file_name, "rb" );
	if( input_file != NULL ) {
		if( fseek( input_file, 0L, SEEK_END ) == 0 ) {
			file_length = ftell( input_file );
			if( file_length >= 0 && buffer ) {
				if( fseek( input_file, 0L, SEEK_SET ) == 0 ) {
					bytes_read = fread( buffer, 1, file_length, input_file ); 
					if( bytes_read != file_length ) {
						file_length = -1;
					}
				} else {
					file_length = -1;
				}
			}
		}
		fclose( input_file );
	}
	if( file_length < 0 ) {
		fputs( strerror( errno ), stderr );
		fputs( "\n", stderr );
	}
	return file_length;
}

static long write_file( char *filename, char *buffer, int length ) {
	long count = -1;
	FILE *file = fopen( filename, "wb" );
	if( file != NULL ) {
		count = fwrite( buffer, 1, length, file );
		fclose( file );
	}
	if( count < length ) {
		fputs( strerror( errno ), stderr );
		fputs( "\n", stderr );
		count = -1;
	}
	return count;
}

static void write_int32le( int value, char *dest ) {
	dest[ 0 ] = value & 0xFF;
	dest[ 1 ] = ( value >> 8 ) & 0xFF;
	dest[ 2 ] = ( value >> 16 ) & 0xFF;
	dest[ 3 ] = ( value >> 24 ) & 0xFF;
}

static int xm_to_wav( struct module *module, char *wav ) {
	int mix_buf[ 16384 ];
	int idx, duration, samples, ampl, offset, length = 0;
	struct replay *replay = new_replay( module, 48000, 0 );
	if( replay ) {
		duration = replay_calculate_duration( replay );
		length = duration * 4 + 44;
		if( wav ) {
			printf( "Wave file length: %d bytes.\n", length );
			strcpy( wav, "RIFF" );
			write_int32le( duration * 4 + 36, &wav[ 4 ] );
			strcpy( &wav[ 8 ], "WAVEfmt " );
			write_int32le( 16, &wav[ 16 ] );
#if IBXM_MONO
			write_int32le( 0x00010001, &wav[ 20 ] );
			write_int32le( 48000, &wav[ 24 ] );
			write_int32le( 48000 * 2, &wav[ 28 ] );
			write_int32le( 0x00100004, &wav[ 32 ] );
			strcpy( &wav[ 36 ], "data" );
			write_int32le( duration * 2, &wav[ 40 ] );
#else
			write_int32le( 0x00020001, &wav[ 20 ] );
			write_int32le( 48000, &wav[ 24 ] );
			write_int32le( 48000 * 4, &wav[ 28 ] );
			write_int32le( 0x00100004, &wav[ 32 ] );
			strcpy( &wav[ 36 ], "data" );
			write_int32le( duration * 4, &wav[ 40 ] );
#endif
			replay_seek( replay, 0 );
			offset = 44;
			while( offset < length ) {
				samples = replay_get_audio( replay, mix_buf );
#if !IBXM_MONO
				samples = samples * 2; //because stereo
#endif
				for( idx = 0; idx < samples; idx++ ) {
					ampl = mix_buf[ idx ];
					if( ampl > 32767 ) {
						ampl = 32767;
					}
					if( ampl < -32768 ) {
						ampl = -32768;
					}
					wav[ offset++ ] = ampl & 0xFF;
					wav[ offset++ ] = ( ampl >> 8 ) & 0xFF;
				}
			}
		}
		dispose_replay( replay );
	}
	return length;
}

int main( int argc, char **argv ) {
	int result, length;
	char *input, *output;
	char message[ 64 ] = "";
	struct data data;
	struct module *module;
	result = EXIT_FAILURE;
	if( argc != 3 ) {
		fprintf( stderr, "%s\nUsage: %s input.xm output.wav\n", IBXM_VERSION, argv[ 0 ] );
	} else {
		/* Read module file.*/
		length = read_file( argv[ 1 ], NULL );
		if( length >= 0 ) {
			printf( "Module Data Length: %d bytes.\n", length );
			input = calloc( length, 1 );
			if( input != NULL ) {
				if( read_file( argv[ 1 ], input ) >= 0 ) {
					data.buffer = input;
					data.length = length;
					struct mallinfo mi=mallinfo();
					module = module_load( &data, message );
					struct mallinfo mi2=mallinfo();
					int used=mi2.uordblks-mi.uordblks;
					printf("Memory used loading module: %d bytes (%d K)\n", used, used/1024);
					if( module ) {
						/* Perform conversion. */
						length = xm_to_wav( module, NULL );
						if( length > 0 ) {
							output = calloc( length, 1 );
							if( output != NULL ) {
								xm_to_wav( module, output );
								if( write_file( argv[ 2 ], output, length ) > 0 ) {
									result = EXIT_SUCCESS;
								}
								free( output );
							}
						}
						dispose_module( module );
					} else {
						fputs( message, stderr );
						fputs( "\n", stderr );
					}
				}
				free( input );
			}
		}
	}
	return result;
}
