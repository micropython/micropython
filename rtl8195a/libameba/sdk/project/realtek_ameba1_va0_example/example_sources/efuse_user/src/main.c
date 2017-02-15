#include "diag.h"
#include "hal_efuse.h"

void main(void)
{
	// There are three blocks, each block has 32 bytes.
	u8 CodeWordNum;	// 0~3, 0: 1~8 bytes, 1: 9~16 bytes, 2: 17~24 bytes, 3: 25~32 bytes
	u8 WordEnable;	// 0x0~0xF, 4bits, 1 bit is for 2 bytes, Max: 8 bytes.
					// bit0: 1,2 byte , bit1: 3,4 byte , bit2: 5,6 byte , bit3: 7,8 byte
	u8 content[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	u8 i;
	// Block 1
	u8 text1_0[8] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
	u8 text1_1[8] = {0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
	u8 text1_2[8] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17};
	u8 text1_3[8] = {0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};
	// Block 2
	u8 text2_0[8] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27};
	u8 text2_1[8] = {0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};
	u8 text2_2[8] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37};
	u8 text2_3[8] = {0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F};
	// Block 3
	u8 text3_0[8] = {0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47};
    u8 text3_1[8] = {0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F};
	u8 text3_2[8] = {0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57};
	u8 text3_3[8] = {0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F};

	u8 bWriteBlock1 = 0;
	u8 bWriteBlock2 = 0;
	u8 bWriteBlock3 = 0;

	DBG_8195A("\nefuse user block: Test Start\n");
	//------------------------------------------------//
	//               efuse user block 1               //
	//------------------------------------------------//
	// write efuse user block 1
	if(bWriteBlock1){
		// write 1~8 bytes in block 1
		CodeWordNum = 0;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant1(CodeWordNum, WordEnable, text1_0) == _SUCCESS);
			DBG_8195A("write 1~8 bytes of block 1 success.\n");
		// write 9~16 bytes in block 1
		CodeWordNum = 1;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant1(CodeWordNum, WordEnable, text1_1) == _SUCCESS);
			DBG_8195A("write 9~16 bytes of block 1 success.\n");
		// write 17~24 bytes in block 1
		CodeWordNum = 2;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant1(CodeWordNum, WordEnable, text1_2) == _SUCCESS);
			DBG_8195A("write 17~24 bytes of block 1 success.\n");
		// write 25~32 bytes in block 1
		CodeWordNum = 3;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant1(CodeWordNum, WordEnable, text1_3) == _SUCCESS);
			DBG_8195A("write 25~32 bytes of block 1 success.\n");
	}
	// read efuse user block 1, read 32 bytes once.
	_memset(content, 0, 32);
	ReadEfuseContant1(content);
	DBG_8195A("efuse user block 1 :\n");
	for(i=0; i<32; i++){
		DBG_8195A("[%d]\t0x%02x\n", i+1, content[i]);
	}
	
	//------------------------------------------------//
	//               efuse user block 2               //
	//------------------------------------------------//
	// write efuse user block 2
	if(bWriteBlock2){
		// write 1~8 bytes in block 2
		CodeWordNum = 0;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant2(CodeWordNum, WordEnable, text2_0) == _SUCCESS);
			DBG_8195A("write 1~8 bytes of block 2 success.\n");
		// write 9~16 bytes in block 2
		CodeWordNum = 1;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant2(CodeWordNum, WordEnable, text2_1) == _SUCCESS);
			DBG_8195A("write 9~16 bytes of block 2 success.\n");
		// write 17~24 bytes in block 2
		CodeWordNum = 2;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant2(CodeWordNum, WordEnable, text2_2) == _SUCCESS);
			DBG_8195A("write 17~24 bytes of block 2 success.\n");
		// write 25~32 bytes in block 2
		CodeWordNum = 3;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant2(CodeWordNum, WordEnable, text2_3) == _SUCCESS);
			DBG_8195A("write 25~32 bytes of block 2 success.\n");
	}
	// read efuse user block 2, read 32 bytes once.
	_memset(content, 0, 32);
	ReadEfuseContant2(content);
	DBG_8195A("efuse user block 2 :\n");
	for(i=0; i<32; i++){
		DBG_8195A("[%d]\t0x%02x\n", i+1, content[i]);
	}
	
	//------------------------------------------------//
	//               efuse user block 3                //
	//------------------------------------------------//
	// write efuse user block 3
	if(bWriteBlock3){
		// write 1~8 bytes in block 3
		CodeWordNum = 0;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant3(CodeWordNum, WordEnable, text3_0) == _SUCCESS);
			DBG_8195A("write 1~8 bytes of block 3 success.\n");
		// write 9~16 bytes in block 3
		CodeWordNum = 1;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant3(CodeWordNum, WordEnable, text3_1) == _SUCCESS);
			DBG_8195A("write 9~16 bytes of block 3 success.\n");
		// write 17~24 bytes in block 3
		CodeWordNum = 2;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant3(CodeWordNum, WordEnable, text3_2) == _SUCCESS);
			DBG_8195A("write 17~24 bytes of block 3 success.\n");
		// write 25~32 bytes in block 3
		CodeWordNum = 3;
		WordEnable = 0xF; // max: 8 bytes
		if(WriteEfuseContant3(CodeWordNum, WordEnable, text3_3) == _SUCCESS);
			DBG_8195A("write 25~32 bytes of block 3 success.\n");
	}
	// read efuse user block 3, read 32 bytes once.
	_memset(content, 0, 32);
	ReadEfuseContant3(content);
	DBG_8195A("efuse user block 3 :\n");
	for(i=0; i<32; i++){
		DBG_8195A("[%d]\t0x%02x\n", i+1, content[i]);
	}
	
	DBG_8195A("efuse user block: Test Done\n");
	for(;;);
}