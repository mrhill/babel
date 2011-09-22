#include "mem.h"

void bbMemFreeNull(void** pPtr)
{
    if (*pPtr)
    {
        bbMemFree(*pPtr);
        *pPtr = NULL;
    }
}

void bbMemSwap(void* pDst, void* pSrc, bbU32 size)
{
    bbU32 offs = 0;
    while (offs < size)
    {
        bbU8 const i = ((bbU8*)pSrc)[offs];
        ((bbU8*)pSrc)[offs] = ((bbU8*)pDst)[offs];
        ((bbU8*)pDst)[offs] = i;
        ++offs;
    }
}

#if bbOS != bbOS_PALMOS

void* bbMemAlloc(const bbU32 size)
{
    void* ptr = malloc(size);

    if (ptr == NULL)
    {
        bbErrSet(bbENOMEM);
    }

    return ptr;
}

void* bbMemCalloc(const bbU32 size)
{
    void* ptr = bbMemAlloc(size);
    if (ptr) bbMemClear(ptr, size);
    return ptr;
}

bbERR bbMemRealloc(const bbU32 size, void** pPtr)
{
    if (size == 0)
    {
        if (*pPtr)
        {
            free(*pPtr);
            *pPtr = NULL;
        }
    }
    else
    {
        void* pTmp;
        if ((pTmp = realloc(*pPtr, size)) == NULL)
        {
            return bbErrSet(bbENOMEM);
        }
        *pPtr = pTmp;
    }

    return bbEOK;
}

#else /* bbOS == bbOS_PALMOS */

void* bbMemAlloc(const bbU32 size)
{
    void* ptr = MemPtrNew(size);
    if (ptr == NULL) bbErrSet(bbENOMEM);
    return ptr;
}

bbERR bbMemRealloc(const bbU32 size, void** pPtr)
{
    if (size == 0)
    {
        if (*pPtr)
        {
            MemPtrFree(*pPtr);
            *pPtr = NULL;
        }
    }
    else
    {
        if (*pPtr == NULL)
        {
            if ((*pPtr = MemPtrNew(size)) == NULL)
                return bbErrSet(bbENOMEM);
        }
        else if (MemPtrResize(*pPtr, size) != errNone)
        {
            UInt32 oldsize = MemPtrSize(*pPtr);

            void* pTmp = MemPtrNew(size);
            if (pTmp == NULL) return bbErrSet(bbENOMEM);
            
            bbMemMove( pTmp, *pPtr, oldsize);

            MemPtrFree( *pPtr);
            *pPtr = pTmp;
        }
    }

    return bbEOK;
}

#endif /* bbOS == bbOS_PALMOS */

