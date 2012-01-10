#include "dir.h"
#include "mem.h"
#include "str.h"

#if (bbOS == bbOS_WIN32) || (bbOS == bbOS_WINCE)

typedef struct
{
    HANDLE  handle;
    WCHAR*  pPath;
    bbU8    first;

} bbDirHandle;

bbDIRH bbDirOpen( const bbCHAR* pPath)
{
    bbDIRH handle = bbMemAlloc( sizeof(bbDirHandle));

    if (handle != bbDIRH_INVALID)
    {
        if ((((bbDirHandle*)handle)->pPath = (WCHAR*) bbStrConvMemTo( bbCE_UTF16NE, 0, pPath)) != NULL)
        {
            ((bbDirHandle*)handle)->handle = INVALID_HANDLE_VALUE;
            ((bbDirHandle*)handle)->first = 1;
        }
        else
        {
            bbMemFreeNull( &handle);
        }
    }

    return handle;
}

bbERR bbDirNext(bbDIRH const handle, bbCHAR** const ppPath, bbUINT* const pFlags)
{
    WIN32_FIND_DATAW data;

    if ( ((bbDirHandle*)handle)->first )
    {
        ((bbDirHandle*)handle)->handle = FindFirstFileW( ((bbDirHandle*)handle)->pPath, &data);

        if ( ((bbDirHandle*)handle)->handle == INVALID_HANDLE_VALUE)
        {
            return bbErrSet(bbENOTFOUND); // xxx check for empty dirs
        }

        ((bbDirHandle*)handle)->first = 0;
    }
    else
    {
        if ( FindNextFileW( ((bbDirHandle*)handle)->handle, &data) == 0)
        {
            if (GetLastError()==ERROR_NO_MORE_FILES)
            {
                if (ppPath)
                    *ppPath = NULL;
                return bbEEND;
            }
            else
                return bbErrSet(bbEUK); // xxx error code
        }
    }

    if (pFlags)
    {
        bbUINT flags = 0;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) flags |= bbDIRENTRY_DIR;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)    flags |= bbDIRENTRY_SYSTEM;
        *pFlags = flags;
    }

    if (ppPath)
    {
        if ((*ppPath = bbStrConvMemFrom( bbCE_UTF16NE, 0, (const bbU8*) data.cFileName)) == NULL)
        {
            return bbErrGet();
        }
    }

    return bbEOK;
}

void bbDirClose(bbDIRH handle)
{
    if (handle != bbDIRH_INVALID)
    {
        if (((bbDirHandle*)handle)->handle != INVALID_HANDLE_VALUE)
        {
            FindClose( ((bbDirHandle*)handle)->handle );
        }
        bbMemFree( ((bbDirHandle*)handle)->pPath );
        bbMemFree(handle);
    }
}

#elif bbOS != bbOS_QT

bbDIRH bbDirOpen( const bbCHAR* pPath)
{
    bbErrSet(bbENOTSUP);
    return NULL;
}

bbERR bbDirNext(bbDIRH const handle, bbCHAR** const ppPath, bbUINT* const pFlags)
{
    return bbErrSet(bbENOTSUP);
}

void bbDirClose(bbDIRH handle)
{
}

#endif

