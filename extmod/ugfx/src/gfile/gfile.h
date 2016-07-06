/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gfile/gfile.h
 * @brief   GFILE - File IO Routines header file.
 *
 * @addtogroup GFILE
 *
 * @brief	Module which contains operating system independent file I/O.
 *
 * @{
 */

#ifndef _GFILE_H
#define _GFILE_H

#include "../../gfx.h"

#if GFX_USE_GFILE || defined(__DOXYGEN__)

/*===========================================================================*/
/* Type definitions                                                          */
/*===========================================================================*/

/**
 * @brief	A file pointer
 */

typedef struct GFILE GFILE;
typedef struct gfileList gfileList;

extern GFILE *gfileStdIn;
extern GFILE *gfileStdErr;
extern GFILE *gfileStdOut;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * @brief					Check if file exists
	 * 
	 * @param[in] fname			The file name
	 * 
	 * @return					TRUE if file exists, FALSE otherwise
	 * 
	 * @api
	 */
	bool_t		gfileExists(const char *fname);

	/**
	 * @brief					Delete file
	 * 
	 * @param[in] fname			The file name
	 * 
	 * @return					TRUE on success, FALSE otherwise
	 * 
	 * @api
	 */
	bool_t		gfileDelete(const char *fname);

	/**
	 * @brief					Get the size of a file
	 * @note					Please use @p gfileGetSize() if the file is opened
	 * 
	 * @param[in] fname			The file name
	 * 
	 * @return					File size on success, -1 on error
	 * 
	 * @api
	 */
	long int	gfileGetFilesize(const char *fname);

	/**
	 * @brief					Rename file
	 *
	 * @param[in] oldname		The current file name
	 * @param[in] newname		The new name of the file
	 *
	 * @return					TRUE on success, FALSE otherwise
	 *
	 * @api
	 */
	bool_t		gfileRename(const char *oldname, const char *newname);

	/**
	 * @brief					Open file
	 * @details					A file must be opened before it can be accessed
	 * @details					The resulting GFILE will be used for all functions that access the file.
	 *
	 * @param[in] fname			The file name
	 * @param[in] mode			The mode.
	 *
	 * @return					Valid GFILE on success, 0 otherwise
	 *
	 * @note					The modes follow the c library fopen() standard.
	 * 							The valid modes are:
	 * 							<ul><li>r   - Open for read, the file must exist</li>
	 * 								<li>w   - Open for write, the file is truncated if it exists</li>
	 * 								<li>wx  - Open for write, the file must not exist</li>
	 * 								<li>a   - Open for append, the file is truncated if it exists</li>
	 * 								<li>ax  - Open for append, the file must not exists</li>
	 * 							</ul>
	 * 							The following flags can also be added to the above modes:<br/>
	 * 							<ul><li>+   - Open for both read and write</li>
	 * 								<li>b   - Open as a binary file rather than a text file</li>
	 * 							</ul>
	 * @note					Not all file-systems support all modes. For example, write
	 * 							is not available with the ROM file-system. Similarly few platforms
	 * 							distinguish between binary and text files.
	 * @note					Even though binary vs. text is relevant only for a small number of platforms
	 * 							the "b" flag should always be specified for binary files such as images.
	 * 							This ensures portability to other platforms. The extra flag will be ignored
	 * 							on platforms where it is not relevant.
	 *
	 * @api
	 */	
	GFILE *		gfileOpen(const char *fname, const char *mode);

	/**
	 * @brief					Close file
	 * @details					Closes a file after is has been opened using @p gfileOpen()
	 *
	 * @param[in] f				The file
	 *
	 * @api
	 */
	void		gfileClose(GFILE *f);

	/**
	 * @brief					Read from file
	 * @details					Reads a given amount of bytes from the file
	 * @details					The read/write cursor will not be reset when calling this function
	 *
	 * @param[in] f				The file
	 * @param[out] buf			The buffer in which to save the content that has been read from the file
	 * @param[in] len			Amount of bytes to read
	 *
	 * @return					Amount of bytes read
	 *
	 * @api
	 */
	size_t		gfileRead(GFILE *f, void *buf, size_t len);

	/**
	 * @brief					Write to file
	 * @details					Write a given amount of bytes to the file
	 * @details					The read/write cursor will not be reset when calling this function
	 *
	 * @param[in] f				The file
	 * @param[in] buf			The buffer which contains the content that will be written to the file
	 * @param[in] len			Amount of bytes to write
	 *
	 * @return					Amount of bytes written
	 *
	 * @api
	 */
	size_t		gfileWrite(GFILE *f, const void *buf, size_t len);

	/**
	 * @brief					Get the current position of the read/write cursor
	 *
	 * @param[in] f				The file
	 *
	 * @return					The current position in the file
	 *
	 * @api
	 */
	long int	gfileGetPos(GFILE *f);

	/**
	 * @brief					Set the position of the read/write cursor
	 *
	 * @param[in] f				The file
	 * @param[in] pos			The position to which the cursor will be set
	 *
	 * @return					TRUE on success, FALSE otherwise
	 *
	 * @api
	 */
	bool_t		gfileSetPos(GFILE *f, long int pos);

	/**
	 * @brief					Get the size of file
	 * @note					Please use @p gfileGetFilesize() if the file is not opened
	 *
	 * @param[in] f				The file
	 *
	 * @return					The size of the file
	 *
	 * @api
	 */
	long int	gfileGetSize(GFILE *f);

	/**
	 * @brief					Check for EOF
	 * @details					Checks if the cursor is at the end of the file
	 *
	 * @param[in] f				The file
	 *
	 * @return					TRUE if EOF, FALSE otherwise
	 *
	 * @api
	 */
	bool_t		gfileEOF(GFILE *f);

	/**
	 * @brief					Mount a logical drive (aka partition)
	 *
	 * @details					Not supported by every file system
	 * @details					Currently just one drive at one is supported.
	 *
	 * @param[in] fs			The file system (F for FatFS)
	 * @param[in] drive			The logical drive prefix
	 *
	 * @return					TRUE on success, FALSE otherwise
	 *
	 * @api
	 */
	bool_t gfileMount(char fs, const char *drive);

	/**
	 * @brief					Unmount a logical drive (aka partition)
	 *
	 * @details					Does have no effect if @p gfileMount() as been called before hand
	 *
	 * @param[in] fs			The file system (F for FatFS)
	 * @param[in] drive			The logical drive prefix
	 *
	 * @return					TRUE on success, FALSE otherwise
	 *
	 * @api
	 */
	bool_t gfileUnmount(char fs, const char *drive);

	/**
	 * @brief					Syncs the file object (flushes the buffer)
	 *
	 * @details					Not supported by every file system
	 *
	 * @param[in] f				The file
	 *
	 * @return					TRUE on success, FALSE otherwise
	 *
	 * @api
	 */
	bool_t gfileSync(GFILE *f);

	#if GFILE_NEED_FILELISTS || defined(__DOXYGEN__)
		/**
		 * @brief				Open a file list
		 *
		 * @param[in] fs		The file system (F for FatFS)
		 * @param[in] path		Path information to pass to the file system
		 * @param[in] dirs		Pass TRUE to get directories only, FALSE to get files only
		 *
		 * @return				A pointer to a file list on success, NULL otherwise
		 *
		 * @note				The path parameter is handled in a file-system specific way. It could be
		 * 						treated as a directory name, it may be treated as a file pattern, or it
		 * 						may be ignored. Passing NULL will always return the full list of files
		 * 						in at least the top level directory.
		 * @note				For file systems that do not support directories, passing TRUE for dirs
		 * 						will return an error.
		 * @note				You must call @p gfileCloseFileList() when you have finished with the
		 * 						file list in order to free resources.
		 *
		 * @api
		 */
		gfileList *gfileOpenFileList(char fs, const char *path, bool_t dirs);

		/**
		 * @brief				Get the next file in a file list.
		 *
		 * @param[in] pfl		Pointer to a file list returned by @p gfileOpenFileList()
		 *
		 * @return				A pointer to a file (or directory) name. Returns NULL if there are no more.
		 *
		 * @note				The file name may contain the full directory path or may not depending
		 * 						on how the file system treats directories.
		 * @note				The returned buffer may be destroyed by the next call to any of
		 * 						@p gfileOpenFileList(), @p gfileReadFileList() or @p gfileCloseFileList().
		 * 						Do not use this pointer after one of those calls.
		 *
		 * @api
		 */
		const char *gfileReadFileList(gfileList *pfl);

		/**
		 * @brief				Close a file list.
		 *
		 * @param[in] pfl		Pointer to a file list returned by @p gfileOpenFileList()
		 *
		 * @api
		 */
		void gfileCloseFileList(gfileList *pfl);
	#endif

	#if (GFILE_NEED_CHIBIOSFS && GFX_USE_OS_CHIBIOS) || defined(__DOXYGEN__)
		/**
		 * @brief					Open file from a ChibiOS FileStream
		 *
		 * @param[in] FileStreamPtr	The BaseFileStream (ChibiOS V2) or FileStream (ChibiOS V3) to open as a GFILE
		 * @param[in] mode			The mode.
		 *
		 * @return					Valid GFILE on success, 0 otherwise
		 *
		 * @note					The modes are the same modes as in @p gfileOpen(). The
		 * 							open mode is NOT compared against the FileStream capabilities.
		 * @note					Supported operations are: read, write, getpos, setpos, eof and getsize
		 *
		 * @api
		 */
		GFILE *		gfileOpenChibiOSFileStream(void *FileStreamPtr, const char *mode);
		#define gfileOpenBaseFileStream(f,m)	gfileOpenChibiOSFileStream(f,m)
	#endif

	#if GFILE_NEED_MEMFS || defined(__DOXYGEN__)
		/**
		 * @brief					Open file from a memory pointer
		 *
		 * @param[in] memptr		The pointer to the memory
		 * @param[in] mode			The mode.
		 *
		 * @return					Valid GFILE on success, 0 otherwise
		 *
		 * @note					The modes are the same modes as in @p gfileOpen(). Note there is
		 * 							no concept of file-size. Be careful not to overwrite other memory or
		 * 							to read from inaccessible sections of memory.
		 * @note					Supported operations are: read, write, getpos, setpos
		 *
		 * @api
		 */
		GFILE *		gfileOpenMemory(void *memptr, const char *mode);
	#endif

	#if GFILE_NEED_STRINGS || defined(__DOXYGEN__)
		/**
		 * @brief					Open file from a null terminated C string
		 *
		 * @param[in] str			The pointer to the string or string buffer
		 * @param[in] mode			The mode
		 *
		 * @return					Valid GFILE on success, 0 otherwise
		 *
		 * @note					The modes are the same modes as in @p gfileOpen(). Note there is
		 * 							no concept of file-size. Be careful not to overwrite other memory or
		 * 							to read from inaccessible sections of memory.
		 * @note					Reading will return EOF when the NULL character is reached.
		 * @note					Writing will always place a NULL in the next character effectively terminating the
		 * 							string at the character just written.
		 * @note					Supported operations are: read, write, append, getpos, setpos
		 * @note					Be careful with setpos and getpos. They do not check for the end of the string.
		 * @note					Reading and Writing will read/write a maximum of one character at a time.
		 *
		 * @api
		 */
		GFILE *		gfileOpenString(char *str, const char *mode);
	#endif

	#if GFILE_NEED_PRINTG || defined(__DOXYGEN__)
		#include <stdarg.h>

		int vfnprintg(GFILE *f, int maxlen, const char *fmt, va_list arg);
		int fnprintg(GFILE *f, int maxlen, const char *fmt, ...);
		#define vfprintg(f,m,a)			vfnprintg(f,0,m,a)
		#define fprintg(f,m,...)		fnprintg(f,0,m,__VA_ARGS__)
		#define vprintg(m,a)			vfnprintg(gfileStdOut,0,m,a)
		#define printg(m,...)			fnprintg(gfileStdOut,0,m,__VA_ARGS__)

		#if GFILE_NEED_STRINGS
			int vsnprintg(char *buf, int maxlen, const char *fmt, va_list arg);
			int snprintg(char *buf, int maxlen, const char *fmt, ...);
			#define vsprintg(s,m,a)		vsnprintg(s,0,m,a)
			#define sprintg(s,m,...)	snprintg(s,0,m,__VA_ARGS__)
		#endif
	#endif

	#if GFILE_NEED_SCANG || defined(__DOXYGEN__)
		#include <stdarg.h>

		int vfscang(GFILE *f, const char *fmt, va_list arg);
		int fscang(GFILE *f, const char *fmt, ...);
		#define vscang(f,a)			vfscang(gfileStdIn,f,a)
		#define scang(f,...)		fscang(gfileStdIn,f,__VA_ARGS__)

		#if GFILE_NEED_STRINGS
			int vsscang(const char *buf, const char *fmt, va_list arg);
			int sscang(const char *buf, const char *fmt, ...);
		#endif
	#endif

	#if GFILE_NEED_STDIO && !defined(GFILE_NEED_STDIO_MUST_BE_OFF)
		// Needed routines and definitions
		size_t gstdioRead(void * ptr, size_t size, size_t count, GFILE *f);
		size_t gstdioWrite(const void * ptr, size_t size, size_t count, GFILE *f);
		int gstdioGetpos(GFILE *f, long int *pos);
		int gstdioSeek(GFILE *f, size_t offset, int origin);
		#define SEEK_SET	0
		#define SEEK_CUR	1
		#define SEEK_END	2

		// Stdio emulation
		#define stdin					gfileStdIn
		#define stdout					gfileStdOut
		#define stderr					gfileStdErr
		#define FOPEN_MAX				GFILE_MAX_GFILES
		#define TMP_MAX					GFILE_MAX_GFILES
		#define FILENAME_MAX			256						// Use a relatively small number for an embedded platform
		#define L_tmpnam				FILENAME_MAX
		#define P_tmpdir				"/tmp/"
		#define FILE					GFILE
		#define fopen(n,m)				gfileOpen(n,m)
		#define fclose(f)				gfileClose(f)
		#define fread(p,sz,cnt,f)		gstdioRead(p,sz,cnt,f)
		#define fwrite(p,sz,cnt,f)		gstdioWrite(p,sz,cnt,f)
		#define fseek(f,ofs,org)		gstdioSeek(f,ofs,org)
		#define remove(n)				(!gfileDelete(n))
		#define rename(o,n)				(!gfileRename(o,n))
		#define fflush(f)				(0)
		#define ftell(f)				gfileGetPos(f)
		#define fpos_t					long int
		#define fgetpos(f,pos)			gstdioGetpos(f,pos)
		#define fsetpos(f, pos)			(!gfileSetPos(f, *pos))
		#define rewind(f)				gfileSetPos(f, 0);
		#define feof(f)					gfileEOF(f)
		#define vfprintf(f,m,a)			vfnprintg(f,0,m,a)
		#define fprintf(f,m,...)		fnprintg(f,0,m,__VA_ARGS__)
		#define vprintf(m,a)			vfnprintg(gfileStdOut,0,m,a)
		#define printf(m,...)			fnprintg(gfileStdOut,0,m,__VA_ARGS__)
		#define vsnprintf(s,n,m,a)		vsnprintg(s,n,m,a)
		#define snprintf(s,n,m,...)		snprintg(s,n,m,__VA_ARGS__)
		#define vsprintf(s,m,a)			vsnprintg(s,0,m,a)
		#define sprintf(s,m,...)		snprintg(s,0,m,__VA_ARGS__)

		//TODO
		//void clearerr ( FILE * stream );
		//int ferror ( FILE * stream );
		//FILE * tmpfile ( void );		// Auto-deleting
		//char * tmpnam ( char * str );
		//char * mktemp (char *template);
		//FILE * freopen ( const char * filename, const char * mode, FILE * stream );
		//setbuf
		//setvbuf
		//fflush
		//fgetc
		//fgets
		//fputc
		//fputs
		//getc
		//getchar
		//puts
		//ungetc
		//void perror (const char * str);
	#endif

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GFILE */

#endif /* _GFILE_H */
/** @} */

