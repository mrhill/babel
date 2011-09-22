#ifndef bbBABEL_FILE_H_
#define bbBABEL_FILE_H_

/** @file
    File I/O.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** File handle.
    NULL if invalid.
    @see bbFileOpen
*/
#if bbOS != bbOS_PALMOS
typedef FILE* bbFILEH;
#else
typedef void* bbFILEH;
#endif

/** Flag for bbFileOpen, open file for reading. */
#define bbFILEOPEN_READ 0x0
/** Flag for bbFileOpen, open file for writing and reading. bbFileOpen will fail if file not existent. */
#define bbFILEOPEN_READWRITE 0x1
/** Flag for bbFileOpen, creates or truncates file to size 0. Must be used together with #bbFILEOPEN_READWRITE. */
#define bbFILEOPEN_TRUNC 0x2
/** Flag for bbFileOpen, creates file if not existent. Must be used together with #bbFILEOPEN_READWRITE. */
#define bbFILEOPEN_CREATE 0x4

/** Seek mode "absolute offset" for #bbFileSeek. */
#define bbFILESEEK_SET 0
/** Seek mode "relative to current positon" for #bbFileSeek. */
#define bbFILESEEK_CUR 1
/** Seek mode "relative to file end" for #bbFileSeek. */
#define bbFILESEEK_END 2

#define bbFILESTAT_TYPEMASK _S_IFMT  /**< file type mask */
#define bbFILESTAT_DIR      _S_IFDIR /**< directory */
#define bbFILESTAT_REG      _S_IFREG /**< regular */
#define bbFILESTAT_READ     0000400  /**< read permission, owner */
#define bbFILESTAT_WRITE    0000200  /**< write permission, owner */
#define bbFILESTAT_EXEC     0000100  /**< execute/search permission, owner */

/** bbFileStat() stat info structure. */
typedef struct
{
    bbU64 size;     //!< File size
    bbU16 mode;     //!< File flags bitmask, see bbFILESTAT
    bbU16 unused;   //!< don't use
    bbU16 uid;      //!< User ID
    bbU16 gid;      //!< Group ID
    bbS32 atime;    //!< Last access time, 0 if invalid
    bbS32 mtime;    //!< Last modification time, always valid
    bbS32 ctime;    //!< Creation time, 0 if invalid
    bbU32 drive;    //!< Drive number
    
} bbFILESTAT;

/** Get file status.
    @param Pointer to 0-terminated pathname
    @param pStat Returns status on success, can be NULL
    @return bbEOK on success, bbEFILENOTFOUND (actual return value) or other error
*/
bbERR bbFileStat(const bbCHAR* const pPath, bbFILESTAT* const pStat);

/** Open a file in specified access mode.
    Sets #bbgErr on failure.
    @param pFilename Path to file
    @param flags     ORed combination of flags. 0 means open as read only.
    @return Handle on success or NULL on failure.
*/
bbFILEH bbFileOpen(const bbCHAR* pFilename, bbUINT flags);

/** Close a file.
    @param handle File handle, can be NULL.
*/
void bbFileClose(bbFILEH handle);

/** Read file size.
    Sets #bbgErr on failure.
    @param handle Valid file handle
    @return Size in bytes or (bbU64)-1 on error
    @return bbEOK on success, or value of #bbgErr on failure.
*/
bbU64 bbFileExt(bbFILEH handle);

#if (bbOS == bbOS_WINCE) || (bbOS == bbOS_WIN32)
#if _MSC_VER < 1000
int __cdecl _fseeki64(FILE*, __int64 , int);
__int64 __cdecl _ftelli64(FILE*);
#endif
#endif

/** Read current file position.
    @param handle (bbFILEH) Valid file handle
    @return (bbU64) Position in bytes or (bbU64)-1 on error
*/
#if (bbOS == bbOS_WINCE) || (bbOS == bbOS_WIN32)
#define bbFileTell(handle) _ftelli64(handle)
#else
#define bbFileTell(handle) (bbU64)(bbS64)ftell(handle)
#endif

/** Move read/write file point to specified position.
    Sets #bbgErr on failure.
    @param handle Valid file handle
    @param offset file offset, interpreted according to mode
    @param mode Interpretation of parameter \a offset.
                Must be one of the following constants:
                <table>
                <tr><td>bbFILESEEK_SET</td><td>Seek to absolute file position, offset is bbU64.</td></tr>
                <tr><td>bbFILESEEK_CUR</td><td>Seek relative to current file position, offset is bbS64.</td></tr>
                <tr><td>bbFILESEEK_END</td><td>Seek relative to file end, offset is bbS64.</td></tr>
                <table>
    @return bbEOK on success, or value of #bbgErr on failure.
*/
bbERR bbFileSeek(bbFILEH handle, const bbS64 offset, const bbUINT mode);

/** Read a block from an open file and forward file pointer.
    Sets #bbgErr on failure.
    @param handle Valid file handle
    @param pBuf   Address to read data to. Enough memory must be ensured.
    @param size   Number of bytes to read.
    @return bbEOK on success, or value of #bbgErr on failure.
    @retval bbEFILEREAD Read operation not completed successfully
*/
bbERR bbFileRead(bbFILEH handle, void* pBuf, const bbU32 size);

/** Write a block from memory to the current file pointer of an open file and forward file pointer.
    Sets #bbgErr on failure.
    @param handle Valid file handle
    @param pBuf   Address of data to write.
    @param size   Number of bytes to write.
    @return bbEOK on success, or value of #bbgErr on failure.
    @retval bbEFILEWRITE Write operation not completed successfully
*/
bbERR bbFileWrite(bbFILEH handle, const void* pBuf, const bbU32 size);

/** Write a value in little endian byte order to the current file pointer of an open file and forward file pointer.
    Sets #bbgErr on failure.
    @param handle Valid file handle
    @param val    Value to write. Write starts from lowest byte.
    @param size   Number of bytes to write (0, 1, 2, 3, or 4).
    @return bbEOK on success, or value of #bbgErr on failure.
    @retval bbEFILEWRITE Write operation not completed successfully
*/
bbERR bbFileWriteLE(bbFILEH handle, const bbU32 val, const bbUINT size);

/** Truncate file at current file pointer.
    Sets #bbgErr on failure.
    @param handle Valid file handle
    @return bbEOK on success, or value of #bbgErr on failure.
*/
bbERR bbFileTrunc(bbFILEH handle);

/** Delete a file or directory.
    @param pPath Path to file
*/
bbERR bbFileDelete(const bbCHAR* const pPath);

/** Rename a file or directory.
    @param pPath Old path
    @param pNewPath New path
*/
bbERR bbFileRename(const bbCHAR* const pPath, const bbCHAR* const pNewPath);

/** Join path from components.
    Sets #bbgErr on failure.
    @param pDir 0-terminated directory name.
    @param pFile 0-terminated file name.
    @param pExt 0-terminated file extension name.
    @return Returns pointer to heap block containing 0-terminated path name on success,
            on NULL on failure. It is the responsibility of the caller to free the
            returned heap block.
*/
bbCHAR* bbPathJoin(const bbCHAR* const pDir, const bbCHAR* const pFile, const bbCHAR* const pExt);

/** Split path into components.
    The pointers referenced via \a ppDir, \a ppFile, and \a ppExt will be set to
    heap blocks containing a 0-terminated string. It is the responsibility of the
    caller to free these blocks later. All 3 pointers can be set to NULL if not
    required.

    If the function fails, #bbgErr is set, and \a *ppDir, \a *ppFile, and \a *ppExt are
    set to NULL.

    @param pPath 0-terminated path. 
    @param ppDir  Returns 0-terminated directory name in heap block, NULL to ignore.
    @param ppFile Returns 0-terminated file name in heap block, NULL to ignore.
    @param ppExt  Returns 0-terminated file extension name in heap block, NULL to ignore.
*/
bbERR bbPathSplit(const bbCHAR* const pPath, bbCHAR** const ppDir, bbCHAR** const ppFile, bbCHAR** const ppExt);

/** Terminate path with directory delimitter.
    @param pPath 0-terminated path string, will be copied and optionally appended with bbDIRDELIM. NULL on error.
*/
bbCHAR* bbPathDelim(const bbCHAR* const pPath);

/** Terminate path with directory delimitter, and reserve extra space in string buffer.
    @param pPath 0-terminated path string, will be copied and optionally appended with bbDIRDELIM. NULL on error.
    @param extralen Number of chars to allocate additionally
*/
bbCHAR* bbPathDelimEx(const bbCHAR* const pPath, bbUINT extralen);

/** Normalize file or directory path.
    @param pPath 0-terminated path
    @return Normalized absolute path, 0-terminated in heap block, must be freed externally
*/
bbCHAR* bbPathNorm(const bbCHAR* const pPath);

/** Return filename of non-existent file.
    @param pDir Path to generate for, or NULL for system temp directory.
    @return Heap block containing 0-terminated filename, must be freed externally using bbMemFree()
*/
bbCHAR* bbPathTemp(const bbCHAR* pDir);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_FILE_H_ */

