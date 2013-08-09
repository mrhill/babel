#include "map.h"
#include "str.h"

struct bbMapPair
{
    const bbCHAR* key;
    void* val;
};

#define bbMAP_KEYCHUNKSIZE (256 - sizeof(void*))

struct bbMapKeyChunk
{
    struct bbMapKeyChunk* next;
    bbCHAR str[bbMAP_KEYCHUNKSIZE];
};

int bbCmp_Str2MapPair(const void *p1, const void *p2)
{
    return bbStrCmp((const bbCHAR*)p1, ((const bbMapPair*)p2)->key);
}

int bbCmpI_Str2MapPair(const void *p1, const void *p2)
{
    return bbStrICmp((const bbCHAR*)p1, ((const bbMapPair*)p2)->key);
}

void bbMapInit(struct bbMap* pMap)
{
    bbMemClear(pMap, sizeof(bbMap));
    pMap->mCmpFn = bbCmp_Str2MapPair;
    pMap->mKeyChunkUsed = bbMAP_KEYCHUNKSIZE;
}

void bbMapDestroy(struct bbMap* pMap)
{
    bbMapDump(pMap);
    bbMapKeyChunk* pChunk = pMap->mpKeys;
    while(pChunk)
    {
        bbMapKeyChunk* pNext = pChunk->next;
        bbMemFree(pChunk);
        pChunk = pNext;
    }

    bbMemFree(pMap->mpPairs);

    bbMemClear(pMap, sizeof(struct bbMap));
}

static bbMapKeyChunk* bbMapAddKeyChunk(struct bbMap* pMap, bbUINT keylen)
{
    if ((int)(keylen -= bbMAP_KEYCHUNKSIZE) < 0)
        keylen = 0;

    bbMapKeyChunk* pNew = bbMemAlloc(sizeof(bbMapKeyChunk) + keylen);
    if (pNew)
    {
        pNew->next = pMap->mpKeys;
        pMap->mpKeys = pNew;
        pMap->mKeyChunkUsed = 0;
    }

    return pNew;
}

static const bbCHAR* bbMapAddKey(struct bbMap* pMap, const bbCHAR* pKey)
{
    const bbCHAR* pKeyCopy;
    bbUINT keylen = bbStrLen(pKey) + 1;

    if ((pMap->mKeyChunkUsed + keylen) > bbMAP_KEYCHUNKSIZE)
    {
        if (!bbMapAddKeyChunk(pMap, keylen))
            return NULL;
    }

    pKeyCopy = pMap->mpKeys->str + pMap->mKeyChunkUsed;
    bbStrCpy(pKeyCopy, pKey);
    pMap->mKeyChunkUsed += keylen;

    return pKeyCopy;
}

bbERR bbMapAddC(struct bbMap* pMap, const bbCHAR* pKey, void* val)
{
    bbMapPair* pInsert = bbBSearchGE(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), pMap->mCmpFn);
    bbUINT insertAt = pInsert - pMap->mpPairs;

    if ((insertAt >= pMap->mSize) || ((*pMap->mCmpFn)(pKey, pInsert) != 0))
    {
        bbUINT newSize = pMap->mSize + 1;
        if (newSize > pMap->mCapacity)
        {
            bbUINT newCapacity = pMap->mCapacity;
            newCapacity = newCapacity ? newCapacity<<1 : 4;

            if (bbMemRealloc(newCapacity * sizeof(bbMapPair), (void**)&pMap->mpPairs) != bbEOK)
                return bbELAST;
            pMap->mCapacity = newCapacity;

            pInsert = pMap->mpPairs + insertAt;
        }

        bbMemMove(pInsert + 1, pInsert, (pMap->mSize - insertAt) * sizeof(bbMapPair));
        pMap->mSize = newSize;
        pInsert->key = pKey;
    }
    pInsert->val = val;

    return bbEOK;
}

bbERR bbMapAdd(struct bbMap* pMap, const bbCHAR* pKey, void* val)
{
    const bbCHAR* pKeyCopy = bbMapAddKey(pMap, pKey);
    if (!pKeyCopy)
        return bbELAST;

    if (bbMapAddC(pMap, pKeyCopy, val) != bbEOK)
        return bbELAST;

    return bbEOK;
}

void* bbMapGet(const struct bbMap* pMap, const bbCHAR* pKey)
{
    bbMapPair* pFound = NULL;

    if (pKey)
        pFound = bbBSearch(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), pMap->mCmpFn);

    if (pFound)
    {
        bbErrSet(bbEOK);
        return pFound->val;
    }

    bbErrSet(bbENOTFOUND);
    return NULL;
}

static int bbMapIsKeyExternal(const struct bbMap* pMap, const bbCHAR* pKey)
{
    int external = 1;
    const bbMapKeyChunk* pChunk = pMap->mpKeys;
    while(pChunk)
    {
        if ((bbUPTR)pKey - (bbUPTR)pChunk < sizeof(bbMapKeyChunk))
        {
            external = 0;
            break;
        }
        pChunk = pChunk->next;
    }
    return external;
}

void bbMapDump(const struct bbMap* pMap)
{
    bbUINT i = 0;
    bbPrintf("bbMap mSize:%u, mCapacity:%u, mKeyChunkUsed=%u\n", pMap->mSize, pMap->mCapacity, pMap->mKeyChunkUsed);

    const bbMapKeyChunk* pChunk = pMap->mpKeys;
    while(pChunk)
    {
        bbPrintf("keychunk %d: %s\n", i++, pChunk->str);
        pChunk = pChunk->next;
    }

    for(i=0; i<pMap->mSize; i++)
        bbPrintf("%s:%p %d\n", pMap->mpPairs[i].key, pMap->mpPairs[i].val, bbMapIsKeyExternal(pMap, pMap->mpPairs[i].key));

}

