#include "mem.h"
#include "str.h"
#include "file.h"

#if (bbOS != bbOS_PALMOS) && (bbOS != bbOS_QT)

#include <errno.h>
#include <wchar.h>
#if defined(ANDROID)
#include <asm/io.h>
#elif (bbOS == bbOS_POSIX)
#include <sys/io.h>
#else
#include <io.h>
#endif

static bbERR errno2bbERR(const bbERR defaulterr);

bbCHAR* bbPathNorm(const bbCHAR* const pPath)
{
    #if (bbOS == bbOS_WINCE) || (bbOS == bbOS_WIN32) || (bbOS == bbOS_WIN64)

    bbU32 len0, len;
    bbCHAR* pStr = NULL;
    bbCHAR* pPath0 = NULL;

    len0 = bbStrLen(pPath);
    if (len0 < 1024)
        len0 = 1024;
    pPath0 = bbStrAlloc(len0);
    if (!pPath0)
        return NULL;
    bbStrCpy(pPath0, pPath);

    len = GetLongPathName(pPath, pPath0, len0);
    if (len > len0)
    {
        if (bbEOK != bbMemRealloc(len*sizeof(bbCHAR), (void**)&pPath0))
            goto bbPathNorm_err0;
        
        len0 = GetLongPathName(pPath, pPath0, len);
    }

    pStr = bbStrAlloc(1024);
    if (!pStr)
        goto bbPathNorm_err0;

    len = GetFullPathName(pPath0, 1024+1, pStr, NULL);
    if (!len)
        goto bbPathNorm_err;

    if (len > 1024+1)
    {
        bbMemFree(pStr);
        pStr = bbStrAlloc(len);
        if (!pStr)
            goto bbPathNorm_err0;

        len = GetFullPathName(pPath, len, pStr, NULL);
        if (!len)
            goto bbPathNorm_err;
    }

    bbMemFree(pPath0);
    bbMemRealloc(sizeof(bbCHAR)*(len+1), (void**)&pStr);
    return pStr;

    bbPathNorm_err:
    bbErrSet(bbESYS);
    bbPathNorm_err0:
    bbMemFree(pStr);
    bbMemFree(pPath0);
    return NULL;

    #else
    return bbStrDup(pPath);
    #endif
}

bbCHAR* bbPathTemp(const bbCHAR* pDir)
{
    bbCHAR* pName;
    bbUINT len;
    bbU32 i, r;

    if (!pDir)
    {
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WIN64) || (bbOS == bbOS_WINCE)
        #if bbENC == bbENC_UTF8
        pName = _tempnam(NULL, NULL);
        #else
        pName = _wtempnam(NULL, NULL);
        #endif
        if (!pName)
            bbErrSet(bbESYS);
        return pName;
#else
        pDir = ".";
#endif
    }

    if ((pName = bbPathDelimEx(pDir, 8)) == NULL)
        return NULL;
    len = bbStrLen(pName);

    i = 0;
    r = 0x2342;
    for(;;)
    {
        bbSprintf(pName + len, bbT("%06X"), r & (0xFFFFFF));
        if (bbEFILENOTFOUND == bbFileStat(pName, NULL))
            break;

        r = ((r<<3)|(r>>(32-3))) ^ r;

        if (++i == 256)
        {
            bbMemFreeNull((void**)&pName);
            break;
        }
    }
    return pName;
}

bbERR bbFileStat(const bbCHAR* const pPath, bbFILESTAT* const pStat)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WIN64) || (bbOS == bbOS_WINCE)
    struct _stati64 s;

    #if bbSIZEOF_CHAR == 2
    if (_wstati64(pPath, &s))
    #else
    if (_stati64(pPath, &s))
    #endif
    {
        return errno2bbERR(bbEUK);
    }

    if (pStat)
    {
        pStat->size = s.st_size;
        *(bbU64*)&pStat->mode = *(bbU64*)&s.st_mode;
        pStat->drive = s.st_dev;
        *(bbU64*)&pStat->atime = *(bbU64*)&s.st_atime;
        pStat->ctime = (bbS32)s.st_ctime;
    }
    return bbEOK;
#else
    return bbErrSet(bbENOTSUP);
#endif
}

static bbERR errno2bbERR(const bbERR defaulterr)
{
    bbERR err = defaulterr;
    if ((errno==EPERM) || 
        (errno==EACCES) || 
        (errno==EROFS) )
        err = bbEFILEACCESS;
    else if (errno==ENOENT)
        err = bbEFILENOTFOUND;
    else if (errno==EEXIST)
        err = bbEFILEEXISTS;
    else if (errno==EINVAL)
        err = bbEBADPARAM;
    return bbErrSet(err);
}

bbERR bbFileRename(const bbCHAR* const pPath, const bbCHAR* const pNewPath)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WIN64) || (bbOS == bbOS_WINCE)
#if bbSIZEOF_CHAR == 2
    if (_wrename(pPath, pNewPath))
#else
    if (rename(pPath, pNewPath))
#endif
        return errno2bbERR(bbEUK);
    return bbEOK;
#else
    return bbErrSet(bbENOTSUP);
#endif
}

bbERR bbFileDelete(const bbCHAR* const pPath)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WIN64) || (bbOS == bbOS_WINCE)
#if bbSIZEOF_CHAR == 2
    if (_wremove(pPath))
#else
    if (remove(pPath))
#endif
        return errno2bbERR(bbEUK);
    return bbEOK;
#else
    return bbErrSet(bbENOTSUP);
#endif
}

bbFILEH bbFileOpen(const bbCHAR* pFilename, bbUINT flags)
{
    bbFILEH handle;
    bbCHAR access[4] = { 'r', 'b', 0, 0 };

    if ((flags & bbFILEOPEN_CREATE) && (bbFileStat(pFilename, NULL) == bbEFILENOTFOUND))
        return bbFileOpen(pFilename, bbFILEOPEN_READWRITE|bbFILEOPEN_TRUNC);

    if (flags & bbFILEOPEN_READWRITE) access[1] = '+', access[2] = 'b';
    if (flags & bbFILEOPEN_TRUNC) access[0] = 'w';

    #if bbSIZEOF_CHAR==1
    if ((handle = fopen( (const char*)pFilename, (const char*)access)) == NULL)
    #else
    if ((handle = _wfopen( pFilename, access)) == NULL)
    #endif
    {
        errno2bbERR(bbEFILEOPEN);
    }

    if (handle && (bbEOK != bbFileSeek(handle, 0, bbFILESEEK_SET)))
    {
        bbFileClose(handle);
        return NULL;
    }

    return handle;
}

void bbFileClose(bbFILEH handle)
{
    if (handle) fclose(handle);
}

bbU64 bbFileExt(bbFILEH handle)
{
    /* xxx make a better implementation */
    bbU64 size;

    const bbU64 pos = bbFileTell(handle);
    if (pos == (bbU64)-1)
        return pos;

    bbFileSeek(handle, 0, bbFILESEEK_END);
    size = bbFileTell(handle);
    bbFileSeek(handle, pos, bbFILESEEK_SET);
    return size;
}

bbERR bbFileSeek(bbFILEH handle, const bbS64 offset, const bbUINT mode)
{
    #if (bbFILESEEK_SET==SEEK_SET) && (bbFILESEEK_CUR==SEEK_CUR) && (bbFILESEEK_END==SEEK_END)

    #if (bbOS == bbOS_WINCE) || (bbOS == bbOS_WIN32)
    if (!_fseeki64( handle, offset, mode))
        return bbEOK;
    #else
    if (!fseek(handle, offset, mode))
        return bbEOK;
    #endif

    return bbErrSet(bbEFILESEEK);

    #else
    #error Seek mode constants bbFILESEEK_* do not match libc definitions.
    #endif
}

bbERR bbFileRead(bbFILEH handle, void* pBuf, const bbU32 size)
{
    if (fread(pBuf, 1, size, handle) == size) 
        return bbEOK;

    if (feof(handle))
        return bbErrSet(bbEEOF);
    else
        return bbErrSet(bbEFILEREAD);
}

bbERR bbFileWrite(bbFILEH handle, const void* pBuf, const bbU32 size)
{
    if (fwrite(pBuf, 1, size, handle) == size)
        return bbEOK;

    return bbErrSet(bbEFILEWRITE);
}

bbERR bbFileTrunc(bbFILEH handle)
{
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WIN64) || (bbOS == bbOS_WINCE)
    int fd = _fileno(handle);
    bbU64 pos = bbFileTell(handle);

    if ((fd == -1) || (pos > (bbU32)-1>>1))
        return bbEUK;

    if (_chsize(fd, (long)pos))
        return errno2bbERR(bbEUK);
    return bbEOK;
#else
    return bbErrSet(bbENOTSUP);
#endif
}

#endif /* bbOS != bbOS_PALMOS */

bbERR bbFileWriteLE(bbFILEH handle, const bbU32 val, const bbUINT size)
{
    #if bbCPUE==bbCPUE_LE /* little endian version */

    return bbFileWrite(handle, (void*)&val, size);

    #else /* big endian version */

    bbU8 buf[4];
    bbST32LE(&buf[0], val)
    return bbFileWrite(handle, &buf[0], size);

    #endif
}

bbCHAR* bbPathJoin(const bbCHAR* const pDir, const bbCHAR* const pFile, const bbCHAR* const pExt)
{
    const bbUINT slen_dir  = pDir  ? bbStrLen(pDir)  : 0;
    const bbUINT slen_file = pFile ? bbStrLen(pFile) : 0;
    const bbUINT slen_ext  = pExt  ? bbStrLen(pExt)  : 0;

    bbCHAR* pPath = (bbCHAR*) bbMemAlloc((slen_dir + slen_file + slen_ext + (2*bbCP_MAXCU + 1))*sizeof(bbCHAR));

    if (pPath)
    {
        bbCHAR* pTmp = pPath;

        if (pDir)
        {
            bbStrCpy( pTmp, pDir); pTmp += slen_dir;
            bbCP_APPEND_PTR(pTmp, bbDIRDELIM);
        }

        if (pFile)
        {
            bbStrCpy( pTmp, pFile); pTmp += slen_file;
        }

        if (pExt)
        {
            bbCP_APPEND_PTR(pTmp, bbFILEEXTDELIM);
            bbStrCpy( pTmp, pExt); pTmp += slen_ext;
        }

        *pTmp = 0;
    }

    return pPath;
}

bbERR bbPathSplit(const bbCHAR* const pPath, bbCHAR** const ppDir, bbCHAR** const ppFile, bbCHAR** const ppExt)
{
    bbCHARCP cp;

    const bbCHAR* pTmp  = pPath;

    const bbCHAR* pSrcDir  = pPath;
    const bbCHAR* pSrcFile = NULL;
    const bbCHAR* pSrcExt  = NULL;

    bbCHAR* pDstDir  = NULL;
    bbCHAR* pDstFile = NULL;
    bbCHAR* pDstExt  = NULL;

    bbUINT len;

    if (!pPath)
        return bbErrSet(bbEBADPARAM);

    do
    {
        bbCP_NEXT_PTR(pTmp, cp);

        if (cp == bbDIRDELIM) pSrcExt = NULL, pSrcFile = pTmp;
        if (cp == bbFILEEXTDELIM) pSrcExt = pTmp;
    } while (cp);

    if (pSrcFile == NULL)
    {
        // not file delim found -> whole string is filename
        pSrcFile = pPath;
        pSrcDir  = NULL;
    }

    if (ppDir)
    {
        len = (pSrcDir==NULL) ? sizeof(bbCHAR) : ((bbUINT)(bbUPTR)pSrcFile - (bbUINT)(bbUPTR)pPath);

        if ((pDstDir = (bbCHAR*) bbMemAlloc((bbU32)len)) == NULL)
            goto bbPathSplit_err;

        len -= sizeof(bbCHAR);
        bbMemMove(pDstDir, pPath, len);
        *(bbCHAR*)((bbU8*)pDstDir + len) = 0;

        *ppDir = pDstDir;
    }

    if (ppFile)
    {
        len = pSrcExt ? (bbUINT)(bbUPTR)pSrcExt - (bbUINT)(bbUPTR)pSrcFile : (bbUINT)(bbUPTR)pTmp - (bbUINT)(bbUPTR)pSrcFile;
        if ((pDstFile = (bbCHAR*)bbMemAlloc((bbU32)len)) == NULL)
            goto bbPathSplit_err;

        len -= sizeof(bbCHAR);
        bbMemMove(pDstFile, pSrcFile, len);
        *(bbCHAR*)((bbU8*)pDstFile + len) = 0;

        *ppFile = pDstFile;
    }

    if (ppExt)
    {
        len = pSrcExt ? (bbUINT)(bbUPTR)pTmp - (bbUINT)(bbUPTR)pSrcExt : sizeof(bbCHAR);
        if ((pDstExt = (bbCHAR*)bbMemAlloc((bbU32)len)) == NULL)
            goto bbPathSplit_err;

        len -= sizeof(bbCHAR);
        bbMemMove(pDstExt, pSrcExt, len);
        *(bbCHAR*)((bbU8*)pDstExt + len) = 0;

        *ppExt = pDstExt;
    }

    return bbEOK;

    bbPathSplit_err:

    if (pDstExt)  bbMemFree(pDstExt);
    if (pDstFile) bbMemFree(pDstFile);
    if (pDstDir)  bbMemFree(pDstDir);

    return bbELAST;
}

bbCHAR* bbPathDelimEx(const bbCHAR* const pPath, bbUINT extralen)
{
    bbUINT len = bbStrLen(pPath);
   
    bbCHAR* pTmp = (bbCHAR*) bbMemAlloc((len + extralen + 2) * sizeof(bbCHAR));

    if (pTmp)
    {
        bbStrCpy(pTmp, pPath);
#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WIN64) || (bbOS == bbOS_WINCE)
        if ((pTmp[len-1] != bbDIRDELIM) && (pTmp[len-1] != '/'))
#else
        if (pTmp[len-1] != bbDIRDELIM)
#endif
        {
            pTmp[len] = bbDIRDELIM;
            pTmp[len+1] = 0;
        }
    }

    return pTmp;
}

bbCHAR* bbPathDelim(const bbCHAR* const pPath)
{
    return bbPathDelimEx(pPath, 0);
}
