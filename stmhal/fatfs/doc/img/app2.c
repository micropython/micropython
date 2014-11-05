/*------------------------------------------------------------/
/ Remove all contents of a directory
/ This function works regardless of _FS_RPATH.
/------------------------------------------------------------*/


FRESULT empty_directory (
    char* path      /* Working buffer filled with start directory */
)
{
    UINT i, j;
    FRESULT fr;
    DIR dir;
    FILINFO fno;

#if _USE_LFN
    fno.lfname = 0; /* Eliminate LFN output */
#endif
    fr = f_opendir(&dir, path);
    if (fr == FR_OK) {
        for (i = 0; path[i]; i++) ;
        path[i++] = '/';
        for (;;) {
            fr = f_readdir(&dir, &fno);
            if (fr != FR_OK || !fno.fname[0]) break;
            if (fno.fname[0] == '.') continue;
            j = 0;
            do
                path[i+j] = fno.fname[j];
            while (fno.fname[j++]);
            if (fno.fattrib & AM_DIR) {
                fr = empty_directory(path);
                if (fr != FR_OK) break;
            }
            fr = f_unlink(path);
            if (fr != FR_OK) break;
        }
        path[--i] = '\0';
        closedir(&dir);
    }

    return fr;
}



int main (void)
{
    FRESULT fr;
    FATFS fs;
    char buff[64];    /* Working buffer */



    f_mount(&fs, "", 0);

    strcpy(buff, "/");  /* Directory to be emptied */
    fr = empty_directory(buff);

    if (fr) {
        printf("Function failed. (%u)\n", fr);
        return 1;
    } else {
        printf("All contents in the %s are successfully removed.\n", buff);
        return 0;
    }
}



