/*
 * fs.h - CC31xx/CC32xx Host Driver Implementation
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "simplelink.h"
    
#ifndef __FS_H__
#define __FS_H__



#ifdef __cplusplus
extern "C" {
#endif

/*!

    \addtogroup FileSystem
    @{

*/

/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/

/* FS error codes */
#define SL_FS_OK                                              (0)
#define SL_FS_ERR_EMPTY_SFLASH                                (-67)
#define SL_FS_ERR_FILE_IS_NOT_SECURE_AND_SIGN                 (-66)
#define SL_FS_ERASING_FLASH                                   (-65)
#define SL_FS_FILE_HAS_NOT_BEEN_CLOSE_CORRECTLY               (-64)
#define SL_FS_WRONG_SIGNATURE                                 (-63)
#define SL_FS_WRONG_SIGNATURE_OR_CERTIFIC_NAME_LENGTH         (-62)
#define SL_FS_NOT_16_ALIGNED                                  (-61)
#define SL_FS_CERT_CHAIN_ERROR                                (-60)
#define SL_FS_FILE_NAME_EXIST                                 (-59)
#define SL_FS_SECURITY_BUF_ALREADY_ALLOC                      (-58)
#define SL_FS_SECURE_FILE_MUST_BE_COMMIT                      (-57)
#define SL_FS_ERR_INCORRECT_OFFSET_ALIGNMENT                  (-56)
#define SL_FS_ERR_FAILED_READ_NVMEM_HEADER                    (-55)
#define SL_FS_WRONG_FILE_NAME                                 (-54)
#define SL_FS_FILE_SYSTEM_IS_LOCKED                           (-53)
#define SL_FS_SECURITY_ALLERT                                 (-52)
#define SL_FS_FILE_UNVALID_FILE_SIZE                          (-51)
#define SL_FS_ERR_TOKEN_IS_NOT_VALID                          (-50)
#define SL_FS_NO_DEVICE_IS_LOADED                             (-49)
#define SL_FS_DATA_ADDRESS_SHOUD_BE_IN_DATA_RAM               (-48)
#define SL_FS_DATA_IS_NOT_ALIGNED                             (-47)
#define SL_FS_ERR_OVERLAP_DETECTION_THRESHHOLD                (-46)
#define SL_FS_FILE_HAS_RESERVED_NV_INDEX                      (-45)
#define SL_FS_ERR_MAX_FS_FILES_IS_LARGER                      (-44)
#define SL_FS_ERR_MAX_FS_FILES_IS_SMALLER                     (-43)
#define SL_FS_FILE_MAX_SIZE_EXCEEDED                          (-42)
#define SL_FS_INVALID_BUFFER_FOR_READ                         (-41)
#define SL_FS_INVALID_BUFFER_FOR_WRITE                        (-40)
#define SL_FS_ERR_FILE_IMAGE_IS_CORRUPTED                     (-39)
#define SL_FS_ERR_SIZE_OF_FILE_EXT_EXCEEDED                   (-38)
#define SL_FS_WARNING_FILE_NAME_NOT_KEPT                      (-37)
#define SL_FS_ERR_DEVICE_IS_NOT_FORMATTED                     (-36)
#define SL_FS_ERR_FAILED_WRITE_NVMEM_HEADER                   (-35)
#define SL_FS_ERR_NO_AVAILABLE_NV_INDEX                       (-34)
#define SL_FS_ERR_FAILED_TO_ALLOCATE_MEM                      (-33)
#define SL_FS_ERR_FAILED_TO_READ_INTEGRITY_HEADER_2           (-32)
#define SL_FS_ERR_FAILED_TO_READ_INTEGRITY_HEADER_1           (-31)
#define SL_FS_ERR_NO_AVAILABLE_BLOCKS                         (-30)
#define SL_FS_ERR_FILE_MAX_SIZE_BIGGER_THAN_EXISTING_FILE     (-29)
#define SL_FS_ERR_FILE_EXISTS_ON_DIFFERENT_DEVICE_ID          (-28)
#define SL_FS_ERR_INVALID_ACCESS_TYPE                         (-27)
#define SL_FS_ERR_FILE_ALREADY_EXISTS                         (-26)
#define SL_FS_ERR_PROGRAM                                     (-25)
#define SL_FS_ERR_NO_ENTRIES_AVAILABLE                        (-24)
#define SL_FS_ERR_FILE_ACCESS_IS_DIFFERENT                    (-23)
#define SL_FS_ERR_BAD_FILE_MODE                               (-22)
#define SL_FS_ERR_FAILED_READ_NVFILE                          (-21)
#define SL_FS_ERR_FAILED_INIT_STORAGE                         (-20)
#define SL_FS_ERR_CONTINUE_WRITE_MUST_BE_MOD_4                (-19)
#define SL_FS_ERR_FAILED_LOAD_FILE                            (-18)
#define SL_FS_ERR_INVALID_HANDLE                              (-17)
#define SL_FS_ERR_FAILED_TO_WRITE                             (-16)
#define SL_FS_ERR_OFFSET_OUT_OF_RANGE                         (-15)
#define SL_FS_ERR_ALLOC                                       (-14)
#define SL_FS_ERR_READ_DATA_LENGTH                            (-13)
#define SL_FS_ERR_INVALID_FILE_ID                             (-12)
#define SL_FS_ERR_FILE_NOT_EXISTS                             (-11)
#define SL_FS_ERR_EMPTY_ERROR                                 (-10)
#define SL_FS_ERR_INVALID_ARGS                                (-9)
#define SL_FS_ERR_FAILED_TO_CREATE_FILE                       (-8)
#define SL_FS_ERR_FS_ALREADY_LOADED                           (-7)
#define SL_FS_ERR_UNKNOWN                                     (-6)
#define SL_FS_ERR_FAILED_TO_CREATE_LOCK_OBJ                   (-5)
#define SL_FS_ERR_DEVICE_NOT_LOADED                           (-4)
#define SL_FS_ERR_INVALID_MAGIC_NUM                           (-3)
#define SL_FS_ERR_FAILED_TO_READ                              (-2)
#define SL_FS_ERR_NOT_SUPPORTED                               (-1)
/* end of error codes */

#define _FS_MODE_ACCESS_RESERVED_OFFSET                       (24)
#define _FS_MODE_ACCESS_RESERVED_MASK                         (0xFF)
#define _FS_MODE_ACCESS_FLAGS_OFFSET                          (16)
#define _FS_MODE_ACCESS_FLAGS_MASK                            (0xFF)
#define _FS_MODE_ACCESS_OFFSET                                (12)
#define _FS_MODE_ACCESS_MASK                                  (0xF)
#define _FS_MODE_OPEN_SIZE_GRAN_OFFSET                        (8)
#define _FS_MODE_OPEN_SIZE_GRAN_MASK                          (0xF)
#define _FS_MODE_OPEN_SIZE_OFFSET                             (0)
#define _FS_MODE_OPEN_SIZE_MASK                               (0xFF)
#define MAX_MODE_SIZE                                         (0xFF)
#define _FS_MODE(Access, SizeGran, Size,Flags)        (_u32)(((_u32)((Access) & _FS_MODE_ACCESS_MASK)<<_FS_MODE_ACCESS_OFFSET) |  \
                                                            ((_u32)((SizeGran) & _FS_MODE_OPEN_SIZE_GRAN_MASK)<<_FS_MODE_OPEN_SIZE_GRAN_OFFSET) | \
                                                            ((_u32)((Size) & _FS_MODE_OPEN_SIZE_MASK)<<_FS_MODE_OPEN_SIZE_OFFSET) | \
                                                            ((_u32)((Flags) & _FS_MODE_ACCESS_FLAGS_MASK)<<_FS_MODE_ACCESS_FLAGS_OFFSET))


/*  sl_FsOpen options */
/*  Open for Read */
#define FS_MODE_OPEN_READ                                     _FS_MODE(_FS_MODE_OPEN_READ,0,0,0)
/*  Open for Write (in case file exist) */
#define FS_MODE_OPEN_WRITE                                    _FS_MODE(_FS_MODE_OPEN_WRITE,0,0,0)
/* Open for Creating a new file */
#define FS_MODE_OPEN_CREATE(maxSizeInBytes,accessModeFlags)   _sl_GetCreateFsMode(maxSizeInBytes,accessModeFlags)        

/*****************************************************************************/
/* Structure/Enum declarations                                               */
/*****************************************************************************/
typedef struct
{
    _u16 flags;
    _u32  FileLen;
    _u32  AllocatedLen;
    _u32  Token[4];
}SlFsFileInfo_t;

typedef enum
{
       _FS_MODE_OPEN_READ            = 0,
       _FS_MODE_OPEN_WRITE,
       _FS_MODE_OPEN_CREATE,
       _FS_MODE_OPEN_WRITE_CREATE_IF_NOT_EXIST
}SlFsFileOpenAccessType_e;

typedef enum
{
   _FS_FILE_OPEN_FLAG_COMMIT  =  0x1,           /* MIRROR - for fail safe */
   _FS_FILE_OPEN_FLAG_SECURE  =  0x2,           /* SECURE */
   _FS_FILE_OPEN_FLAG_NO_SIGNATURE_TEST = 0x4,  /* Relevant to secure file only  */
   _FS_FILE_OPEN_FLAG_STATIC  =           0x8,  /*  Relevant to secure file only */
   _FS_FILE_OPEN_FLAG_VENDOR  =           0x10, /*  Relevant to secure file only */
   _FS_FILE_PUBLIC_WRITE=                 0x20, /* Relevant to secure file only, the file can be opened for write without Token */
   _FS_FILE_PUBLIC_READ =                 0x40  /* Relevant to secure file only, the file can be opened for read without Token  */
}SlFileOpenFlags_e;

typedef enum
{
       _FS_MODE_SIZE_GRAN_256B    = 0,   /*  MAX_SIZE = 64K  */
       _FS_MODE_SIZE_GRAN_1KB,           /*  MAX_SIZE = 256K */
       _FS_MODE_SIZE_GRAN_4KB,           /*  MAX_SZIE = 1M   */
       _FS_MODE_SIZE_GRAN_16KB,          /*  MAX_SIZE = 4M   */
       _FS_MODE_SIZE_GRAN_64KB,          /*  MAX_SIZE = 16M  */
       _FS_MAX_MODE_SIZE_GRAN
}_SlFsFileOpenMaxSizeGran_e;

/*****************************************************************************/
/* Internal Function prototypes                                              */
/*****************************************************************************/
_u32 _sl_GetCreateFsMode(_u32 maxSizeInBytes,_u32 accessFlags);

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

/*!
    \brief open file for read or write from/to storage device
    
    \param[in]      pFileName                  File Name buffer pointer  
    \param[in]      AccessModeAndMaxSize       Options: As described below
    \param[in]      pToken                     Reserved for future use. Use NULL for this field
    \param[out]     pFileHandle      Pointing on the file and used for read and write commands to the file     
     
     AccessModeAndMaxSize possible input                                                                        \n
     FS_MODE_OPEN_READ                                        - Read a file                                                                  \n
     FS_MODE_OPEN_WRITE                                       - Open for write for an existing file                                          \n
     FS_MODE_OPEN_CREATE(maxSizeInBytes,accessModeFlags)      - Open for creating a new file. Max file size is defined in bytes.             \n
                                                                For optimal FS size, use max size in 4K-512 bytes steps (e.g. 3584,7680,117760)  \n
                                                                Several access modes bits can be combined together from SlFileOpenFlags_e enum

    \return         On success, zero is returned. On error, an error code is returned    
    
    \sa             sl_FsRead sl_FsWrite sl_FsClose       
    \note           belongs to \ref basic_api       
    \warning        
    \par            Example:
    \code
       char*           DeviceFileName = "MyFile.txt";
       unsigned long   MaxSize = 63 * 1024; //62.5K is max file size
       long            DeviceFileHandle = -1;
       long            RetVal;        //negative retval is an error
       unsigned long   Offset = 0;
       unsigned char   InputBuffer[100];

       // Create a file and write data. The file in this example is secured, without signature and with a fail safe commit
       RetVal = sl_FsOpen((unsigned char *)DeviceFileName,
                                        FS_MODE_OPEN_CREATE(MaxSize , _FS_FILE_OPEN_FLAG_NO_SIGNATURE_TEST | _FS_FILE_OPEN_FLAG_COMMIT ),
                                        NULL, &DeviceFileHandle);

       Offset = 0;
       //Preferred in secure file that the Offset and the length will be aligned to 16 bytes.
       RetVal = sl_FsWrite( DeviceFileHandle, Offset, (unsigned char *)"HelloWorld", strlen("HelloWorld"));

       RetVal = sl_FsClose(DeviceFileHandle, NULL, NULL , 0);

       // open the same file for read, using the Token we got from the creation procedure above
       RetVal = sl_FsOpen((unsigned char *)DeviceFileName,
                                        FS_MODE_OPEN_READ,
                                        NULL, &DeviceFileHandle);

       Offset = 0;
       RetVal = sl_FsRead( DeviceFileHandle, Offset, (unsigned char *)InputBuffer, strlen("HelloWorld"));

       RetVal = sl_FsClose(DeviceFileHandle, NULL, NULL , 0);

     \endcode
*/
#if _SL_INCLUDE_FUNC(sl_FsOpen)
_i32 sl_FsOpen(const _u8 *pFileName,const _u32 AccessModeAndMaxSize,_u32 *pToken,_i32 *pFileHandle);
#endif

/*!
    \brief close file in storage device
    
    \param[in]      FileHdl                 Pointer to the file (assigned from sl_FsOpen) 
    \param[in]      pCeritificateFileName   Reserved for future use. Use NULL.
    \param[in]      pSignature              Reserved for future use. Use NULL.
    \param[in]      SignatureLen            Reserved for future use. Use 0.
                    

    \return         On success, zero is returned.  On error, an error code is returned   
    
    \sa             sl_FsRead sl_FsWrite sl_FsOpen        
    \note           Call the fs_Close  with signature = 'A' signature len = 1 for activating an abort action
    \warning
    \par            Example:
    \code            
    sl_FsClose(FileHandle,0,0,0);
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_FsClose)
_i16 sl_FsClose(const _i32 FileHdl,const _u8* pCeritificateFileName,const _u8* pSignature,const _u32 SignatureLen);
#endif

/*!
    \brief Read block of data from a file in storage device
    
    \param[in]      FileHdl Pointer to the file (assigned from sl_FsOpen)    
    \param[in]      Offset  Offset to specific read block
    \param[out]     pData   Pointer for the received data
    \param[in]      Len     Length of the received data
     
    \return         On success, returns the number of read bytes. On error, negative number is returned    
    
    \sa             sl_FsClose sl_FsWrite sl_FsOpen        
    \note           belongs to \ref basic_api       
    \warning     
    \par            Example:
    \code    
    Status = sl_FsRead(FileHandle, 0, &readBuff[0], readSize);
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_FsRead)
_i32 sl_FsRead(const _i32 FileHdl,_u32 Offset ,_u8*  pData,_u32 Len);
#endif

/*!
    \brief write block of data to a file in storage device
    
    \param[in]      FileHdl  Pointer to the file (assigned from sl_FsOpen)  
    \param[in]      Offset   Offset to specific block to be written
    \param[in]      pData    Pointer the transmitted data to the storage device
    \param[in]      Len      Length of the transmitted data
     
    \return         On success, returns the number of written bytes.  On error, an error code is returned
    
    \sa                     
    \note           belongs to \ref basic_api       
    \warning     
    \par            Example:
    \code    
    Status = sl_FsWrite(FileHandle, 0, &buff[0], readSize);
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_FsWrite)
_i32 sl_FsWrite(const _i32 FileHdl,_u32 Offset,_u8*  pData,_u32 Len);
#endif

/*!
    \brief get info on a file
    
    \param[in]      pFileName    File name
    \param[in]      Token        Reserved for future use. Use 0
    \param[out]     pFsFileInfo Returns the File's Information: flags,file size, allocated size and Tokens 
     
    \return         On success, zero is returned.   On error, an error code is returned    
    
    \sa             sl_FsOpen        
    \note           belongs to \ref basic_api       
    \warning        
    \par            Example:
    \code    
    Status = sl_FsGetInfo("FileName.html",0,&FsFileInfo);
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_FsGetInfo)
_i16 sl_FsGetInfo(const _u8 *pFileName,const _u32 Token,SlFsFileInfo_t* pFsFileInfo);
#endif

/*!
    \brief Delete specific file from a storage or all files from a storage (format)
    
    \param[in]      pFileName    File Name 
    \param[in]      Token        Reserved for future use. Use 0
    \return         On success, zero is returned.  On error, an error code is returned    
    
    \sa                     
    \note           belongs to \ref basic_api       
    \warning     
    \par            Example:
    \code    
    Status = sl_FsDel("FileName.html",0);
    \endcode
*/
#if _SL_INCLUDE_FUNC(sl_FsDel)
_i16 sl_FsDel(const _u8 *pFileName,const _u32 Token);
#endif
/*!

 Close the Doxygen group.
 @}

 */

#ifdef  __cplusplus
}
#endif /*  __cplusplus */

#endif /*  __FS_H__ */

