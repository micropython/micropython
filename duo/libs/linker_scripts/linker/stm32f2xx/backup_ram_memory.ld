/* Declaration for the location of the application and system backup RAM region. */
    BACKUPSRAM (rwx) : ORIGIN = 0x40024000, LENGTH = 4K-4
    BACKUPSRAM_SYSTEM (rwx) : ORIGIN = 0x40025000-4, LENGTH = 4
