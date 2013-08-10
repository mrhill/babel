#include "map.h"
#include "str.h"
#include "fixmath.h"
#include "algo.h"

typedef struct bbMapPair
{
    const bbCHAR* key;
    bbU64PTR val;

} bbMapPair;

#define bbMAP_KEYCHUNKSIZE 256

typedef struct bbMapKeyChunk
{
    struct bbMapKeyChunk* next;
    bbU16 used;
    bbU16 refct;
    bbCHAR str[bbMAP_KEYCHUNKSIZE];

} bbMapKeyChunk;

static int bbCmp_Str2MapPair(const void *p1, const void *p2)
{
    return bbStrCmp((const bbCHAR*)p1, ((const bbMapPair*)p2)->key);
}

void bbMapInit(struct bbMap* pMap)
{
    bbMemClear(pMap, sizeof(bbMap));
}

void bbMapDestroy(struct bbMap* pMap)
{
    bbMapKeyChunk* pChunk = pMap->mpKeyChunk;
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

    bbMapKeyChunk* pNewChunk = bbMemAlloc(sizeof(bbMapKeyChunk) + keylen);
    if (pNewChunk)
    {
        pNewChunk->next = pMap->mpKeyChunk;
        pNewChunk->refct = 0;
        pNewChunk->used = 0;
        pMap->mpKeyChunk = pNewChunk;
    }

    return pNewChunk;
}

static const bbCHAR* bbMapAddKey(struct bbMap* pMap, const bbCHAR* pKey)
{
    const bbCHAR* pKeyCopy;
    bbUINT keylen = bbStrLen(pKey) + 1;

    if (keylen >= 65536) /* bbMapKeyChunk::used is bbU16 */
    {
        bbErrSet(bbEOVERFLOW);
        return NULL;
    }

    bbMapKeyChunk* pKeyChunk = pMap->mpKeyChunk;
    if (!pKeyChunk || (pKeyChunk->used + keylen) > bbMAP_KEYCHUNKSIZE)
    {
        if (!bbMapAddKeyChunk(pMap, keylen))
            return NULL;
        pKeyChunk = pMap->mpKeyChunk;
    }

    pKeyCopy = pKeyChunk->str + pKeyChunk->used;
    pKeyChunk->refct++;
    pKeyChunk->used += keylen;
    bbStrCpy(pKeyCopy, pKey);
    return pKeyCopy;
}

#define bbMapGetCapcity(size) ((size) ? 2 << bbGetTopBit((size)-1) : 0)

static bbMapPair* bbMapInsertOne(struct bbMap* pMap, bbUINT insertAt)
{
    bbMapPair* pInsert;
    bbUINT capacity = bbMapGetCapcity(pMap->mSize);
    bbUINT newSize = pMap->mSize + 1;
    if (newSize > capacity)
    {
        capacity = capacity ? capacity<<1 : 2;
        if (bbMemRealloc(capacity * sizeof(bbMapPair), (void**)&pMap->mpPairs) != bbEOK)
            return NULL;
    }
    pInsert = pMap->mpPairs + insertAt;
    bbMemMove(pInsert + 1, pInsert, (pMap->mSize - insertAt) * sizeof(bbMapPair));
    pMap->mSize = newSize;
    return pInsert;
}

bbERR bbMapAddC(struct bbMap* pMap, const bbCHAR* pKey, bbU64PTR val)
{
    bbMapPair* pInsert = bbBSearchGE(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), bbCmp_Str2MapPair);
    bbUINT insertAt = pInsert - pMap->mpPairs;

    if ((insertAt >= pMap->mSize) || (bbCmp_Str2MapPair(pKey, pInsert) != 0))
    {
        pInsert = bbMapInsertOne(pMap, insertAt);
        if (!pInsert)
            return bbELAST;
        pInsert->key = pKey;
    }

    pInsert->val = val;
    return bbEOK;
}

bbERR bbMapAdd(struct bbMap* pMap, const bbCHAR* pKey, bbU64PTR val)
{
    bbMapPair* pInsert = bbBSearchGE(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), bbCmp_Str2MapPair);
    bbUINT insertAt = pInsert - pMap->mpPairs;

    if ((insertAt >= pMap->mSize) || (bbCmp_Str2MapPair(pKey, pInsert) != 0))
    {
        const bbCHAR* pKeyCopy = bbMapAddKey(pMap, pKey);
        if (!pKeyCopy)
            return bbELAST;

        pInsert = bbMapInsertOne(pMap, insertAt);
        if (!pInsert)
            return bbELAST;

        pInsert->key = pKeyCopy;
    }

    pInsert->val = val;
    return bbEOK;
}

bbU64PTR bbMapGet(const struct bbMap* pMap, const bbCHAR* pKey)
{
    bbMapPair* pFound = NULL;

    if (pKey)
        pFound = bbBSearch(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), bbCmp_Str2MapPair);

    if (pFound)
    {
        bbErrSet(bbEOK);
        return pFound->val;
    }

    bbErrSet(bbENOTFOUND);
    return 0;
}

static void bbMapDerefChunkForKey(struct bbMap* pMap, const bbCHAR* pKey)
{
    bbMapKeyChunk** pParent = &pMap->mpKeyChunk;
    bbMapKeyChunk* pChunk = pMap->mpKeyChunk;
    while(pChunk)
    {
        if (((bbUPTR)pKey - (bbUPTR)pChunk) < sizeof(bbMapKeyChunk))
        {
            if (--pChunk->refct == 0)
            {
                *pParent = pChunk->next;
                bbMemFree(pChunk);
            }
            return;
        }
        pParent = &pChunk->next;
        pChunk = pChunk->next;
    }
}

bbU64PTR bbMapDel(struct bbMap* pMap, const bbCHAR* pKey)
{
    bbU64PTR val = 0;
    bbMapPair* pFound = NULL;

    if (pKey)
        pFound = bbBSearch(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), bbCmp_Str2MapPair);

    if (pFound)
    {
        bbUINT newSize = pMap->mSize - 1;
        bbUINT capacity = bbMapGetCapcity(newSize);
        bbUINT found = pFound - pMap->mpPairs;

        bbErrSet(bbEOK);
        val = pFound->val;

        bbMapDerefChunkForKey(pMap, pFound->key);

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
static int bbMapGetChunkNumberForKey(const struct bbMap* pMap, const bbCHAR* pKey)
{
    int i = 0, chunk = -1;
    const bbMapKeyChunk* pChunk = pMap->mpKeyChunk;
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
    bbU32 datasize = 0, memsize = 0, chunksize;
    const bbMapKeyChunk* pChunk;
    const bbMapKeyChunk** chunkPtrs;
    const bbCHAR** chunkEnds;
    const bbCHAR* pStr;

    bbPrintf("bbMap mSize:%u\n", pMap->mSize);

    for(keychunks = 0, pChunk = pMap->mpKeyChunk; pChunk; pChunk = pChunk->next)
        keychunks++;
    chunkPtrs = bbMemAlloc(keychunks * sizeof(void*));
    chunkEnds = bbMemAlloc(keychunks * sizeof(void*));

    for(i = 0, pChunk = pMap->mpKeyChunk; pChunk; i++, pChunk = pChunk->next)
    {
        chunkPtrs[i] = pChunk;
        chunkEnds[i] = pChunk->str;
    }

    for(i=0; i<pMap->mSize; i++)
    {
        bbUINT keylen = bbStrLen(pMap->mpPairs[i].key) + 1;
        bbUINT keychunk = bbMapGetChunkNumberForKey(pMap, pMap->mpPairs[i].key);

        if (keychunk < keychunks)
        {
            const bbCHAR* pKeyEnd = pMap->mpPairs[i].key + keylen;
            if (chunkEnds[keychunk] < pKeyEnd)
                chunkEnds[keychunk] = pKeyEnd;
            datasize += keylen * sizeof(bbCHAR);
        }

        datasize += sizeof(bbMapPair);
        memsize += sizeof(bbMapPair);
        bbPrintf(" \"%s\":0x%"bbI64"u chunk %d\n", pMap->mpPairs[i].key, pMap->mpPairs[i].val, keychunk);
    }

    for(i=0; i<keychunks; i++)
    {
        pChunk = chunkPtrs[i];
        chunksize = (chunkEnds[i] - pChunk->str) * sizeof(bbCHAR);
        memsize += chunksize;
        bbPrintf(" keychunk %u, size %u, used %u, refct %u, \"", i, chunksize, pChunk->used, pChunk->refct);
        for(pStr = pChunk->str; pStr<chunkEnds[i]; )
            pStr += bbPrintf("%s.", pStr);
        bbPrintf("\"\n");
    }

    bbPrintf(" datasize %u, memsize %u\n", datasize, memsize);

    bbMemFree(chunkEnds);
    bbMemFree(chunkPtrs);
}
#endif

