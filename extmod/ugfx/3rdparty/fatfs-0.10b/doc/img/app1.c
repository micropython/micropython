/*------------------------------------------------------------/
/ Open or create a file in append mode
/------------------------------------------------------------*/

FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const char* path    /* [IN]  File name to be opened */
)
{
    FRESULT fr;

    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
        fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}


int main (void)
{
    FRESULT fr;
    FATFS fs;
    FIL fil;

    /* Open or create a log file and ready to append */
    f_mount(&fs, "", 0);
    fr = open_append(&fil, "logfile.txt");
    if (fr != FR_OK) return 1;

    /* Append a line */
    f_printf(&fil, "%02u/%02u/%u, %2u:%02u\n", Mday, Mon, Year, Hour, Min);

    /* Close the file */
    f_close(&fil);

    return 0;
}

