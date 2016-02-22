#include "FreeRTOS.h"
#include "task.h"
#include "diag.h"
#include "main.h"

#include "i2s_api.h"

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
#include "alc5651.c"
/*
extern void alc5651_init(void);
extern void alc5651_init_interface2(void);
extern void alc5651_reg_dump(void);
extern void alc5651_index_dump(void);
extern void alc5651_set_word_len(int len_idx);
*/
i2s_t i2s_obj;

#define I2S_DMA_PAGE_SIZE	768   // 2 ~ 4096
#define I2S_DMA_PAGE_NUM    4   // Vaild number is 2~4

u8 i2s_tx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];
u8 i2s_rx_buf[I2S_DMA_PAGE_SIZE*I2S_DMA_PAGE_NUM];

#define SAMPLE_FILE
#define SAMPLE_FILE_RATE 44100
#define SAMPLE_FILE_CHNUM 2

#define I2S_SCLK_PIN            PC_1
#define I2S_WS_PIN              PC_0
#define I2S_SD_PIN              PC_2

#if defined(SAMPLE_FILE)
// no sample
//	SR_96KHZ,
//	SR_7p35KHZ,
//	SR_29p4KHZ,
//	SR_88p2KHZ
#if SAMPLE_FILE_RATE==8000
	#if SAMPLE_FILE_CHNUM==2
		#include "birds_8000_2ch_16b.c"
		#undef SAMPLE_FILE_RATE
		#define SAMPLE_FILE_RATE SR_8KHZ
	#endif
#elif SAMPLE_FILE_RATE==11025
	#if SAMPLE_FILE_CHNUM==2
		#include "birds_11025_2ch_16b.c"
		#undef SAMPLE_FILE_RATE
		#define SAMPLE_FILE_RATE SR_11p02KHZ
	#endif
#elif SAMPLE_FILE_RATE==16000
	#if SAMPLE_FILE_CHNUM==2
		#include "birds_16000_2ch_16b.c"
		#undef SAMPLE_FILE_RATE
		#define SAMPLE_FILE_RATE SR_16KHZ
	#endif
#elif SAMPLE_FILE_RATE==22050
	#if SAMPLE_FILE_CHNUM==2
		#include "birds_22050_2ch_16b.c"
		#undef SAMPLE_FILE_RATE
		#define SAMPLE_FILE_RATE SR_22p05KHZ
	#endif
#elif SAMPLE_FILE_RATE==24000
	#if SAMPLE_FILE_CHNUM==2
		#include "birds_24000_2ch_16b.c"
		#undef SAMPLE_FILE_RATE
		#define SAMPLE_FILE_RATE SR_24KHZ
	#endif
#elif SAMPLE_FILE_RATE==32000
	#if SAMPLE_FILE_CHNUM==2
		#include "birds_32000_2ch_16b.c"
		#undef SAMPLE_FILE_RATE
		#define SAMPLE_FILE_RATE SR_32KHZ
	#endif
#elif SAMPLE_FILE_RATE==44100
	#if SAMPLE_FILE_CHNUM==2
		#include "birds_44100_2ch_16b.c"
		#undef SAMPLE_FILE_RATE
		#define SAMPLE_FILE_RATE SR_44p1KHZ
	#endif
#elif SAMPLE_FILE_RATE==48000
	#if SAMPLE_FILE_CHNUM==2
		#include "birds_48000_2ch_16b.c"
		#undef SAMPLE_FILE_RATE
		#define SAMPLE_FILE_RATE SR_48KHZ
	#endif
#endif

#if SAMPLE_FILE_CHNUM==2
	#undef SAMPLE_FILE_CHNUM
	#define SAMPLE_FILE_CHNUM CH_STEREO
#endif

int curr_cnt=0;
#else

short test_sine16[16]={0, 12539/4, 23170/4, 30273/4, 32767/4, 30273/4, 23170/4, 12539/4,
                  0, -12539/4, -23170/4, -30273/4, -32767/4, -30273/4, -23170/4, -12539/4};
int test_sine24[16]={0, 12539*256/4, 23170*256/4, 30273*256/4, 32767*256/4, 30273*256/4, 23170*256/4, 12539*256/4,
                  0, -12539*256/4, -23170*256/4, -30273*256/4, -32767*256/4, -30273*256/4, -23170*256/4, -12539*256/4};

extern void wait_ms(u32);

#include <math.h>
short remap_level_to_signed_16_bit(float val)
{
	val*=32767;
	if(val>32767)	val=32767;
	if(val<-32768)	val=-32768;
	
	return val;
}

void generate_freq_16bit(short *buffer, int count, float freq, float sampling_rate)
{
  int pos; // sample number we're on

  for (pos = 0; pos < count; pos++) {
    float a = 2 * 3.14159f * freq * pos / sampling_rate;
    // convert from [-1.0,1.0] to [-32767,32767]:
    buffer[pos] = remap_level_to_signed_16_bit(a);
  }
}

void gen_sound_sample16(short *buf, int buf_size, int channel_num)
{
	int i;
	for (i = 0 ; i < buf_size ; i+=channel_num){
		buf[i] = test_sine16[(i/channel_num)%16];
		if(channel_num>=2)
        	buf[i+1] = test_sine16[(i/channel_num)%16];
    }
}

void gen_sound_sample24(int *buf, int buf_size, int channel_num)
{
	int i;
	for (i = 0 ; i < buf_size ; i+=channel_num){
		buf[i] = test_sine24[(i/channel_num)%16]&0xFFFFFF;
		if(channel_num>=2)
        	//buf[i+1] = test_sine24[(i/channel_num)%16]&0xFFFFFF;
			buf[i+1] = test_sine24[(i/channel_num)%16]&0xFFFFFF;
    }
}

#if 0
void test_delay(int sec)
{
	for(int i=0;i<166*1000*100*sec;i++)
		asm(" nop");
}
#endif

int test_rate_list[12] = {
    SR_8KHZ, 
    SR_16KHZ,
    SR_24KHZ,
	SR_32KHZ,
    SR_48KHZ,
	SR_96KHZ,
	SR_7p35KHZ,
	SR_11p02KHZ,
	SR_22p05KHZ,
	SR_29p4KHZ,
	SR_44p1KHZ,
	SR_88p2KHZ
};
#endif

void test_tx_complete(void *data, char *pbuf)
{
    int *ptx_buf;
    
    i2s_t *obj = (i2s_t *)data;
    static u32 count=0;
    //DBG_8195A_I2S_LVL(VERI_I2S_LVL, "I2S%d %s\n",pI2SDemoHnd->DevNum,__func__);
    count++;
    if ((count&1023) == 1023)
    {
         DBG_8195A_I2S_LVL(VERI_I2S_LVL, ",\n");
    }

    ptx_buf = i2s_get_tx_page(obj);
    //ptx_buf = (int*)pbuf;
#if defined(SAMPLE_FILE)	
    _memcpy((void*)ptx_buf, (void*)&sample[curr_cnt], I2S_DMA_PAGE_SIZE);
	curr_cnt+=(I2S_DMA_PAGE_SIZE/sizeof(short));
	if(curr_cnt >= sample_size*(obj->channel_num==CH_MONO?1:2)) {
		curr_cnt = 0;
    }
#else
	if(obj->word_length == WL_16b){
		gen_sound_sample16((short*)ptx_buf, I2S_DMA_PAGE_SIZE/sizeof(short), obj->channel_num==CH_MONO?1:2);
	}else{
		gen_sound_sample24((int*)ptx_buf, I2S_DMA_PAGE_SIZE/sizeof(int), obj->channel_num==CH_MONO?1:2);
	}
#endif
    i2s_send_page(obj, (uint32_t*)ptx_buf);
}

void test_rx_complete(void *data, char* pbuf)
{
    i2s_t *obj = (i2s_t *)data;
    int *ptx_buf;

    static u32 count=0;
    count++;
    if ((count&1023) == 1023)
    {
         DBG_8195A_I2S_LVL(VERI_I2S_LVL, ".\n");
    }

    ptx_buf = i2s_get_tx_page(obj);
    _memcpy((void*)ptx_buf, (void*)pbuf, I2S_DMA_PAGE_SIZE);
    i2s_recv_page(obj);    // submit a new page for receive
    i2s_send_page(obj, (uint32_t*)ptx_buf);    // loopback
}

void main(void)
{
    int *ptx_buf;
    int i,j;
    
	alc5651_init();
	alc5651_init_interface2();	// connect to ALC interface 2
	
	// dump register 
	//alc5651_reg_dump();
	//alc5651_index_dump();

	// I2S init
	i2s_obj.channel_num = CH_MONO;//CH_STEREO;
	i2s_obj.sampling_rate = SR_44p1KHZ;
	i2s_obj.word_length = WL_16b;
	i2s_obj.direction = I2S_DIR_TXRX;    
	i2s_init(&i2s_obj, I2S_SCLK_PIN, I2S_WS_PIN, I2S_SD_PIN);
    i2s_set_dma_buffer(&i2s_obj, (char*)i2s_tx_buf, (char*)i2s_rx_buf, \
        I2S_DMA_PAGE_NUM, I2S_DMA_PAGE_SIZE);
    i2s_tx_irq_handler(&i2s_obj, (i2s_irq_handler)test_tx_complete, (uint32_t)&i2s_obj);
    i2s_rx_irq_handler(&i2s_obj, (i2s_irq_handler)test_rx_complete, (uint32_t)&i2s_obj);
    
#if defined(SAMPLE_FILE)	
	i2s_set_param(&i2s_obj,SAMPLE_FILE_CHNUM,SAMPLE_FILE_RATE,WL_16b);
    for (i=0;i<I2S_DMA_PAGE_NUM;i++) {
        ptx_buf = i2s_get_tx_page(&i2s_obj);
        if (ptx_buf) {
            _memcpy((void*)ptx_buf, (void*)&sample[curr_cnt], I2S_DMA_PAGE_SIZE);
            i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
            curr_cnt+=(I2S_DMA_PAGE_SIZE/sizeof(short));
            if(curr_cnt >= sample_size*(i2s_obj.channel_num==CH_MONO?1:2)) {
                curr_cnt = 0;
            }
        }
    }
#else
	// output freq, @ sampling rate
	// 6kHz 		@ 96kHz
	// 3kHz 		@ 48kHz
	// 2kHz			@ 32kHz
	// 1.5kHz 		@ 24kHz
	// 1kHz 		@ 16kHz
	// 500Hz 		@ 8kHz
	// 5512.5 Hz 	@ 88200Hz
	// 2756.25 Hz	@ 44100Hz
	// 1837.5 Hz	@ 29400Hz
	// 1378.125 Hz	@ 22050Hz
	// 459.375 Hz	@ 7350Hz

	// Stereo, 16bit
	for(i=0;i<12;i++){
		i2s_set_param(&i2s_obj,CH_STEREO,test_rate_list[i],WL_16b);
        // Start with fill all pages of DMA buffer
        for (j=0;j<I2S_DMA_PAGE_NUM;j++) {
            ptx_buf = i2s_get_tx_page(&i2s_obj);
            if (ptx_buf) {
                gen_sound_sample16((short*)ptx_buf, I2S_DMA_PAGE_SIZE/sizeof(short), 2);
                i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
            }
        }
        wait_ms(5000);  // delay 5 sec.
	}

	// Mono, 16bit
	for(i=0;i<12;i++){
		i2s_set_param(&i2s_obj,CH_MONO,test_rate_list[i],WL_16b);
        for (j=0;j<I2S_DMA_PAGE_NUM;j++) {
            ptx_buf = i2s_get_tx_page(&i2s_obj);
            if (ptx_buf) {
                gen_sound_sample16((short*)ptx_buf, I2S_DMA_PAGE_SIZE/sizeof(short), 1);
                i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
            }
        }
        wait_ms(5000);  // delay 5 sec.
	}

//	i2s_deinit(&i2s_obj);
	i2s_disable(&i2s_obj);

	alc5651_set_word_len(2);	
	alc5651_reg_dump();
    
	i2s_enable(&i2s_obj);
	// Stereo, 24bit
	for(i=0;i<12;i++){
		i2s_set_param(&i2s_obj,CH_STEREO,test_rate_list[i],WL_24b);
        for (j=0;j<I2S_DMA_PAGE_NUM;j++) {
            ptx_buf = i2s_get_tx_page(&i2s_obj);
            if (ptx_buf) {
                gen_sound_sample24((int*)ptx_buf, I2S_DMA_PAGE_SIZE/sizeof(int), 2);
                i2s_send_page(&i2s_obj, (uint32_t*)ptx_buf);
            }
        }
        wait_ms(5000);  // delay 5 sec.
	}
	
	// Not Support Mono, 24bit
	i2s_deinit(&i2s_obj);
#endif
	
	
	while(1);
}
