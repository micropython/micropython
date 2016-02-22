#include "diag.h"
#include "main.h"

#include "objects.h"
#include "flash_api.h"

// Decide starting flash address for storing application data
// User should pick address carefully to avoid corrupting image section

#define FLASH_APP_BASE  0xFF000

void main(void)
{
    flash_t         flash;
    uint32_t        address = FLASH_APP_BASE;

#if 1
    uint32_t        val32_to_write = 0x13572468;
    uint32_t        val32_to_read;
    int loop = 0;
    int result = 0;
    for(loop = 0; loop < 10; loop++)
    {
        flash_read_word(&flash, address, &val32_to_read);
        DBG_8195A("Read Data 0x%x\n", val32_to_read);
        flash_erase_sector(&flash, address);
        flash_write_word(&flash, address, val32_to_write);
        flash_read_word(&flash, address, &val32_to_read);

        DBG_8195A("Read Data 0x%x\n", val32_to_read);

        // verify result
        result = (val32_to_write == val32_to_read) ? 1 : 0;
        //printf("\r\nResult is %s\r\n", (result) ? "success" : "fail");
        DBG_8195A("\r\nResult is %s\r\n", (result) ? "success" : "fail");
        result = 0;
    }
#else
    int VERIFY_SIZE = 256;
    int SECTOR_SIZE = 16;
    
    uint8_t writedata[VERIFY_SIZE];
    uint8_t readdata[VERIFY_SIZE];
    uint8_t verifydata = 0;
    int loop = 0;
    int index = 0;
    int sectorindex = 0;
    int result = 0;
    int resultsector = 0;
    int testloop = 0;
    for(testloop = 0; testloop < 1; testloop++){
        address = FLASH_APP_BASE;
        for(sectorindex = 0; sectorindex < 4080; sectorindex++){
            result = 0;
            //address += SECTOR_SIZE;
            flash_erase_sector(&flash, address);
            //DBG_8195A("Address = %x \n", address);
            for(loop = 0; loop < SECTOR_SIZE; loop++){
                for(index = 0; index < VERIFY_SIZE; index++)
                {
                    writedata[index] = verifydata + index;
                }
                flash_stream_write(&flash, address, VERIFY_SIZE, &writedata);
                flash_stream_read(&flash, address, VERIFY_SIZE, &readdata);

                for(index = 0; index < VERIFY_SIZE; index++)
                {
                    //DBG_8195A("Address = %x, Writedata = %x, Readdata = %x \n",address,writedata[index],readdata[index]);
                    if(readdata[index] != writedata[index]){
                        DBG_8195A("Error: Loop = %d, Address = %x, Writedata = %x, Readdata = %x \n",testloop,address,writedata[index],readdata[index]);
                    }
                    else{
                        result++;
                        //DBG_8195A(ANSI_COLOR_BLUE"Correct: Loop = %d, Address = %x, Writedata = %x, Readdata = %x \n"ANSI_COLOR_RESET,testloop,address,writedata[index],readdata[index]);
                    }
                }
                address += VERIFY_SIZE;
            }
            if(result == VERIFY_SIZE * SECTOR_SIZE){
                //DBG_8195A("Sector %d Success \n", sectorindex);
                resultsector++;
            }
        }
        if(resultsector == 4079){
            DBG_8195A("Test Loop %d Success \n", testloop);    
        }
        resultsector = 0;
        verifydata++;
    }
    //DBG_8195A("%d Sector Success \n", resultsector);
    DBG_8195A("Test Done");

#endif
    for(;;);
}