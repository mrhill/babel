#include "map.h"
#include "mem.h"
#include "str.h"

void bbMapInit(bbMap* const pMap)
{
    bbMemClear(pMap, sizeof(bbMap));
}

void bbMapDestroy(bbMap* const pMap)
{
    bbMapPair* const pStart = pMap->pKeys;
    bbMapPair* pPair = pStart + pMap->size;

    while(pPair > pStart)
    {
        --pPair;
        if (pPair->alloced)
            bbMemFree((void*)pPair->pKey);
    }

    bbMemFree(pStart);

    #ifdef bbDEBUG
    pMap->pKeys = NULL;
    #endif
}

bbERR bbMapAddC(bbMap* const pMap, const bbCHAR* pKey, void* val)
{
    bbUINT size = pMap->size + 1;
    bbMapPair* pNew;

    if (bbMemRealloc(sizeof(bbMapPair)*size, (void**)&pMap->pKeys) != bbEOK)
        return bbELAST;

    pMap->size = size;
    pNew = &pMap->pKeys[size-1];
    pNew->pKey = pKey;
    pNew->val = val;
    pNew->alloced = 0;

    return bbEOK;
}

bbERR bbMapAdd(bbMap* const pMap, const bbCHAR* pKey, void* val)
{
    bbCHAR* pStr = bbStrDup(pKey);
    if (!pStr)
        return bbELAST;

    if (bbMapAddC(pMap, pStr, val) != bbEOK)
    {
        bbMemFree(pStr);
        return bbELAST;
    }

    pMap->pKeys[pMap->size-1].alloced = 1;

    return bbEOK;
}

void* bbMapGet(bbMap* const pMap, bbCHAR* pKey)
{
    bbMapPair* const pStart = pMap->pKeys;
    bbMapPair* pPair = pStart + pMap->size;

    while(pPair > pStart)
    {
        --pPair;

        if (bbStrICmp(pPair->pKey, pKey) == 0)
        {
            bbErrSet(bbEOK);
            return pPair->val;
        }
    }

    bbErrSet(bbENOTFOUND);
    return NULL;
}

