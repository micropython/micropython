#include <stdio.h>
#include <string.h>
#include <time.h>


#define _DEBUG_FTR	0

#define SECBOOT_IMG_ADDR         (0x2100)
#define SECBOOT_HEADER_LEN    (0x100)
#define SECBOOT_HEADER_POS   (SECBOOT_IMG_ADDR - SECBOOT_HEADER_LEN)
#define SECBOOT_IMG_AREA_TOTAL_LEN   (56*1024)

#define RUN_IMG_HEADER_LEN    (0x100)

#define IMG_HEADER_MAGIC_NO   (0xA0FFFF9F)
#define ONCE_READ_LEN	1024
typedef struct __T_BOOTER
{
	unsigned int   	magic_no;
	unsigned short 	img_type;
	unsigned short 	zip_type;	
	unsigned int   	run_img_addr;
	unsigned int   	run_img_len;
	unsigned int	run_org_checksum;
	unsigned int    upd_img_addr;
	unsigned int    upd_img_len;
	unsigned int 	upd_checksum;
	unsigned int   	upd_no;
	unsigned char  	ver[16];
	unsigned int 	hd_checksum;
}T_BOOTER;

#define OUTPUT_REFLECT 		1
#define INPUT_REFLECT   	2

static const unsigned int crc32_tab[] = { 0x00000000L, 0x77073096L, 0xee0e612cL,
        0x990951baL, 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
        0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL,
        0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L, 0x1db71064L, 0x6ab020f2L,
        0xf3b97148L, 0x84be41deL, 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L,
        0x83d385c7L, 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
        0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L,
        0x4c69105eL, 0xd56041e4L, 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L,
        0xd20d85fdL, 0xa50ab56bL, 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L,
        0xacbcf940L, 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
        0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L,
        0x56b3c423L, 0xcfba9599L, 0xb8bda50fL, 0x2802b89eL, 0x5f058808L,
        0xc60cd9b2L, 0xb10be924L, 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL,
        0xb6662d3dL, 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
        0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L,
        0x0f00f934L, 0x9609a88eL, 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL,
        0x91646c97L, 0xe6635c01L, 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L,
        0xf262004eL, 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
        0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL,
        0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L, 0x4db26158L, 0x3ab551ceL,
        0xa3bc0074L, 0xd4bb30e2L, 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL,
        0xd3d6f4fbL, 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
        0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL,
        0x270241aaL, 0xbe0b1010L, 0xc90c2086L, 0x5768b525L, 0x206f85b3L,
        0xb966d409L, 0xce61e49fL, 0x5edef90eL, 0x29d9c998L, 0xb0d09822L,
        0xc7d7a8b4L, 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
        0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL, 0xead54739L,
        0x9dd277afL, 0x04db2615L, 0x73dc1683L, 0xe3630b12L, 0x94643b84L,
        0x0d6d6a3eL, 0x7a6a5aa8L, 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L,
        0x7d079eb1L, 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
        0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L, 0xfed41b76L,
        0x89d32be0L, 0x10da7a5aL, 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L,
        0x17b7be43L, 0x60b08ed5L, 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L,
        0x4fdff252L, 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
        0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L, 0xdf60efc3L,
        0xa867df55L, 0x316e8eefL, 0x4669be79L, 0xcb61b38cL, 0xbc66831aL,
        0x256fd2a0L, 0x5268e236L, 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L,
        0x5505262fL, 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
        0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L,
        0xec63f226L, 0x756aa39cL, 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL,
        0x72076785L, 0x05005713L, 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL,
        0x0cb61b38L, 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
        0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL,
        0xf6b9265bL, 0x6fb077e1L, 0x18b74777L, 0x88085ae6L, 0xff0f6a70L,
        0x66063bcaL, 0x11010b5cL, 0x8f659effL, 0xf862ae69L, 0x616bffd3L,
        0x166ccf45L, 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
        0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL,
        0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L,
        0x47b2cf7fL, 0x30b5ffe9L, 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L,
        0x24b4a3a6L, 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
        0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L,
        0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL };

unsigned int file_crc = 0xFFFFFFFF;

//位翻转函数
unsigned long long Reflect(unsigned long long ref,unsigned char ch)
{	
	int i;
	unsigned long long value = 0;
	for( i = 1; i < ( ch + 1 ); i++ )
	{
		if( ref & 1 )
			value |= 1 << ( ch - i );
		ref >>= 1;
	}
	return value;
}

/*计算buffer的crc校验码*/
static unsigned int crc32(unsigned int crc,unsigned char *buffer, int size, unsigned char mode)
{
	int i;
	unsigned char temp;
	for (i = 0; i < size; i++) {
		if(mode & INPUT_REFLECT)
		{
			temp = Reflect(buffer[i], 8);
		}
		else
		{
			temp = buffer[i];
		}
		crc = crc32_tab[(crc ^ temp) & 0xff] ^ (crc >> 8);
	}
	return crc ;
}

unsigned int get_crc32(unsigned char *buffer, int size, unsigned char mode)
{
	file_crc = crc32(file_crc, buffer, size, mode);
	if(mode & OUTPUT_REFLECT)
	{
		file_crc = Reflect(file_crc, 32);
	}
	return file_crc;
}

int file_size(char* filename)  
{  
    FILE *fp=fopen(filename,"r");  
    if(!fp) return -1;  
    fseek(fp,0L,SEEK_END);  
    int size=ftell(fp);  
    fclose(fp);
      
    return size;
} 

/********************************************************
Final .FLS File Like this structure:
	------------------------------------
	| T_BOOTER--------.FLS header	           |
	------------------------------------
	| T_BOOTER--------secboot header         |
	------------------------------------
	| 256-sizeof(T_BOOTER) pad 0xFF             |
	------------------------------------
	|SECBOOT-IMG Area-(56*1024 - 256)       |
	------------------------------------
	| T_BOOTER--------image header            |
	------------------------------------
	| 256-sizeof(T_BOOTER)-pad 0xFF            |
	------------------------------------
	| RUN-IMG                                             |
	------------------------------------
*********************************************************/
int main(int argc,char *argv[])
{
	FILE *fpbin = NULL;
	FILE *fpimg = NULL;
	FILE *fout = NULL;
	
	unsigned char buf[ONCE_READ_LEN + 1];	
	int readlen = 0;
	int filelen = 0;
	int i;
	T_BOOTER tbooter;

	int secboot_len = 0;
	int appimg_len = 0;
	int final_len = 0;
	int magic_word = 0;

	memset(&tbooter, 0, sizeof(T_BOOTER));
	//printf("argc:%d\n", argc);	
	switch (argc)
	{		
		case 4:
			break;
		default:
			printf("\nparam cnt error\n");
			printf("参数0:  exe\n");
			printf("参数1:  输入secboot.img文件\n");
			printf("参数2:  输入image文件\n");
			printf("参数3 : 输出文件\n");
			return -1;
			
	}
	fpbin = fopen(argv[1],"rb+"); 
	if(NULL == fpbin)
	{
		printf("\ncan not open input file \n");
		return -2;
	}

	magic_word = 0;
	fread(&magic_word, 1, 4, fpbin);
	if(magic_word != IMG_HEADER_MAGIC_NO)
	{
		printf("\ninput %s file error \n", argv[1]);
		return -3;
	}

	secboot_len = file_size(argv[1]);

	fpimg = fopen(argv[2], "rb+");
	if(NULL == fpimg)
	{
		printf("\nopen img file error\n");
		fclose(fpbin);
		return -4;
	}

	magic_word = 0;
	fread(&magic_word, 1, 4, fpimg);
	if(magic_word != IMG_HEADER_MAGIC_NO)
	{
		printf("\ninput %s file error \n",argv[2]);
		return -4;
	}
	
	fout = fopen(argv[3], "wb+");

	appimg_len = file_size(argv[2]);

	//Write 0xFF to output file
	final_len = SECBOOT_IMG_AREA_TOTAL_LEN + appimg_len;
	for(i = 0; i < (final_len /ONCE_READ_LEN); i++)
	{
		memset(buf, 0xff, ONCE_READ_LEN);
		fwrite(buf, 1, ONCE_READ_LEN, fout);
	}
	memset(buf, 0xff, final_len % ONCE_READ_LEN);
	fwrite(buf, 1, final_len % ONCE_READ_LEN, fout);
	
	printf("secboot_len:%x, app_imglen:%x, total:%x\r\n",secboot_len, appimg_len, final_len);
	
	fseek(fout, sizeof(T_BOOTER), SEEK_SET);
	fseek(fpbin, 0, SEEK_SET);
	//Write SECBOOT header to output file
	readlen = fread(buf, 1, sizeof(T_BOOTER), fpbin);
	fwrite(buf, 1, sizeof(T_BOOTER), fout);
	
	//Write SECBOOT image to output file
	fseek(fout, sizeof(T_BOOTER)+256, SEEK_SET);
	while(!feof(fpbin))
	{
		readlen = fread(buf, 1, ONCE_READ_LEN, fpbin);
		fwrite(buf, 1, readlen, fout);
	}

	//Write run img header to output file
	fseek(fout, sizeof(T_BOOTER) + SECBOOT_IMG_AREA_TOTAL_LEN, SEEK_SET);
	fseek(fpimg, 0, SEEK_SET);
	readlen = fread(buf, 1, sizeof(T_BOOTER), fpimg);
	fwrite(buf, 1, readlen, fout);

	fseek(fout, sizeof(T_BOOTER) + SECBOOT_IMG_AREA_TOTAL_LEN + RUN_IMG_HEADER_LEN, SEEK_SET);
	//Write 	run img to output file
	fseek(fpimg, sizeof(T_BOOTER), SEEK_SET);
	while(!feof(fpimg))
	{
		readlen = fread(buf, 1, ONCE_READ_LEN, fpimg);
		fwrite(buf, 1, readlen, fout);
	}

	//Calculate CRC from secboot header
	file_crc = 0xFFFFFFFF;
	fseek(fout, sizeof(T_BOOTER), SEEK_SET);
	filelen = 0;
	while(!feof(fout))
	{
		readlen = fread(buf, 1, ONCE_READ_LEN, fout);
		filelen += readlen;
		get_crc32((unsigned char*)buf, readlen, 0);			
	}	
	tbooter.magic_no = IMG_HEADER_MAGIC_NO;		
	tbooter.run_org_checksum = file_crc;
	tbooter.img_type = 0;
	tbooter.run_img_len = filelen;
	tbooter.run_img_addr = SECBOOT_HEADER_POS;
	tbooter.zip_type = 0;
	tbooter.upd_img_len = 0;
	tbooter.upd_img_addr = 0;	
	tbooter.upd_checksum = 0;

	//Write fls's header to output file
	file_crc = 0xFFFFFFFF;
	get_crc32((unsigned char *)&tbooter, sizeof(T_BOOTER)-4, 0);
  	tbooter.hd_checksum = file_crc;	
	fseek(fout, 0, SEEK_SET);
	fwrite(&tbooter, 1, sizeof(T_BOOTER), fout);		
	
	if(fpbin)
	{
		fclose(fpbin);
	}
	if(fpimg)
	{
		fclose(fpimg);
	}

	if(fout)
	{
		fclose(fout);
	}
	return 0;
}
