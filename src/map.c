#include "map.h"
#include "str.h"
#include "fixmath.h"

typedef struct bbMapPair
{
    const bbCHAR* key;
    bbU64PTR val;

} bbMapPair;

#define bbMAP_KEYCHUNKSIZE (256 - sizeof(void*))

typedef struct bbMapKeyChunk
{
    struct bbMapKeyChunk* next;
    bbCHAR str[bbMAP_KEYCHUNKSIZE];
} bbMapKeyChunk;

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

#define bbMapGetCapcity(size) ((size) ? 2 << bbGetTopBit((size)-1) : 0)

bbERR bbMapAddC(struct bbMap* pMap, const bbCHAR* pKey, bbU64PTR val)
{
    bbMapPair* pInsert = bbBSearchGE(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), pMap->mCmpFn);
    bbUINT insertAt = pInsert - pMap->mpPairs;

    if ((insertAt >= pMap->mSize) || ((*pMap->mCmpFn)(pKey, pInsert) != 0))
    {
        bbUINT capacity = bbMapGetCapcity(pMap->mSize);
        bbUINT newSize = pMap->mSize + 1;
        if (newSize > capacity)
        {
            capacity = capacity ? capacity<<1 : 2;
            if (bbMemRealloc(capacity * sizeof(bbMapPair), (void**)&pMap->mpPairs) != bbEOK)
                return bbELAST;
            pInsert = pMap->mpPairs + insertAt;
        }

        bbMemMove(pInsert + 1, pInsert, (pMap->mSize - insertAt) * sizeof(bbMapPair));
        pMap->mSize = newSize;
        pInsert->key = pKey;
    }
    pInsert->val = val;

    return bbEOK;
}

bbERR bbMapAdd(struct bbMap* pMap, const bbCHAR* pKey, bbU64PTR val)
{
    const bbCHAR* pKeyCopy = bbMapAddKey(pMap, pKey);
    if (!pKeyCopy)
        return bbELAST;

    if (bbMapAddC(pMap, pKeyCopy, val) != bbEOK)
        return bbELAST;

    return bbEOK;
}

bbU64PTR bbMapGet(const struct bbMap* pMap, const bbCHAR* pKey)
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
    return 0;
}

bbU64PTR bbMapDel(struct bbMap* pMap, const bbCHAR* pKey)
{
    bbU64PTR val = 0;
    bbMapPair* pFound = NULL;

    if (pKey)
        pFound = bbBSearch(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), pMap->mCmpFn);

    if (pFound)
    {
        bbUINT newSize = pMap->mSize - 1;
        bbUINT capacity = bbMapGetCapcity(newSize);
        bbUINT found = pFound - pMap->mpPairs;

        bbErrSet(bbEOK);
        val = pFound->val;

        bbMemMove(pFound, pFound + 1, (newSize - found) * sizeof(bbMapPair));
        bbMemRealloc(capacity * sizeof(bbMapPair), (void**)&pMap->mpPairs);
        pMap->mSize = newSize;
    }
    else
    {
        bbErrSet(bbENOTFOUND);
    }

    return val;
}

void bbMapEnumerate(const struct bbMap* pMap, int (*cb)(const bbCHAR*, bbU64PTR))
{
    bbUINT i = 0;
    for(i=0; i<pMap->mSize; i++)
        if ((*cb)(pMap->mpPairs[i].key, pMap->mpPairs[i].val))
            break;
}

#ifndef bbDEBUG
void bbMapDump(const struct bbMap* pMap) { pMap=pMap; }
#else
static int bbMapGetChunkForKey(const struct bbMap* pMap, const bbCHAR* pKey)
{
    int i = 0, chunk = -1;
    const bbMapKeyChunk* pChunk = pMap->mpKeys;
    while(pChunk)
    {
        if ((bbUPTR)pKey - (bbUPTR)pChunk < sizeof(bbMapKeyChunk))
        {
            chunk = i;
            break;
        }
        pChunk = pChunk->next;
        i++;
    }
    return chunk;
}

void bbMapDump(const struct bbMap* pMap)
{
    bbUINT i = 0, keychunks = 0;
    bbU32 datasize = 0, memsize = 0;
    const bbMapKeyChunk* pChunk;
    const bbCHAR** chunkPtrs;

    bbPrintf("bbMap mSize:%u, mKeyChunkUsed=%u\n", pMap->mSize, pMap->mKeyChunkUsed);

    for(keychunks = 0, pChunk = pMap->mpKeys; pChunk; pChunk = pChunk->next)
        keychunks++;
    chunkPtrs = bbMemAlloc(keychunks * 2 * sizeof(void*));

    for(i = 0, pChunk = pMap->mpKeys; pChunk; i++, pChunk = pChunk->next)
        chunkPtrs[i*2] = chunkPtrs[i*2+1] = pChunk->str;

    for(i=0; i<pMap->mSize; i++)
    {
        bbUINT keylen = bbStrLen(pMap->mpPairs[i].key) + 1;
        bbUINT keychunk = bbMapGetChunkForKey(pMap, pMap->mpPairs[i].key);

        if (keychunk < keychunks)
        {
            const bbCHAR* pKeyEnd = pMap->mpPairs[i].key + keylen;
            if (chunkPtrs[keychunk*2+1] < pKeyEnd)
                chunkPtrs[keychunk*2+1] = pKeyEnd;
            datasize += keylen * sizeof(bbCHAR);
        }

        datasize += sizeof(bbMapPair);
        memsize += sizeof(bbMapPair);
        bbPrintf(" \"%s\":0x%"bbI64"u chunk %d\n", pMap->mpPairs[i].key, pMap->mpPairs[i].val, keychunk);
    }

    for(i=0; i<keychunks; i++)
    {
        bbU32 chunksize = (chunkPtrs[i*2+1] - chunkPtrs[i*2]) * sizeof(bbCHAR);
        memsize += chunksize;
        bbPrintf(" keychunk %u, size %u, \"", i, chunksize);
        while(chunkPtrs[i*2] < chunkPtrs[i*2+1])
            chunkPtrs[i*2] += bbPrintf("%s.", chunkPtrs[i*2]);
        bbPrintf("\"\n");
    }

    bbPrintf(" datasize %u, memsize %u\n", datasize, memsize);
    bbMemFree(chunkPtrs);
}
#endif

