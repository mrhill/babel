#include "map.h"
#include "str.h"
#include "fixmath.h"
#include "algo.h"

#define bbMAP_KEYCHUNKSIZE 256

struct bbMapKeyChunk
{
    struct bbMapKeyChunk* next;
    bbU16 used;
    bbU16 refct;
    bbCHAR str[bbMAP_KEYCHUNKSIZE];

};

static int bbCmp_Str2MapPair(const void *p1, const void *p2)
{
    return bbStrCmp((const bbCHAR*)p1, ((const bbMapPair*)p2)->key);
}

void bbMapInit(bbMapRec* pMap)
{
    bbMemClear(pMap, sizeof(bbMapRec));
}

void bbMapDestroy(bbMapRec* pMap)
{
    struct bbMapKeyChunk* pChunk = pMap->mpKeyChunk;
    while(pChunk)
    {
        struct bbMapKeyChunk* pNext = pChunk->next;
        bbMemFree(pChunk);
        pChunk = pNext;
    }

    bbMemFree(pMap->mpPairs);

    bbMemClear(pMap, sizeof(bbMapRec));
}

static struct bbMapKeyChunk* bbMapAddKeyChunk(bbMapRec* pMap, bbUINT keylen)
{
    struct bbMapKeyChunk* pNewChunk;

    if ((int)(keylen -= bbMAP_KEYCHUNKSIZE) < 0)
        keylen = 0;

    pNewChunk = bbMemAlloc(sizeof(struct bbMapKeyChunk) + keylen);
    if (pNewChunk)
    {
        pNewChunk->next = pMap->mpKeyChunk;
        pNewChunk->refct = 0;
        pNewChunk->used = 0;
        pMap->mpKeyChunk = pNewChunk;
    }

    return pNewChunk;
}

static const bbCHAR* bbMapAddKey(bbMapRec* pMap, const bbCHAR* pKey)
{
    struct bbMapKeyChunk* pKeyChunk;
    const bbCHAR* pKeyCopy;
    bbUINT keylen = bbStrLen(pKey) + 1;

    if (keylen >= 65536) /* bbMapKeyChunk::used is bbU16 */
    {
        bbErrSet(bbEOVERFLOW);
        return NULL;
    }

    pKeyChunk = pMap->mpKeyChunk;
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

static bbMapPair* bbMapInsertOne(bbMapRec* pMap, bbUINT insertAt)
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

int bbMapAddC(bbMapRec* pMap, const bbCHAR* pKey, bbU64PTR val)
{
    bbMapPair* pInsert = bbBSearchGE(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), bbCmp_Str2MapPair);
    bbUINT insertAt = pInsert - pMap->mpPairs;

    if ((insertAt >= pMap->mSize) || (bbCmp_Str2MapPair(pKey, pInsert) != 0))
    {
        pInsert = bbMapInsertOne(pMap, insertAt);
        if (!pInsert)
            return -1;
        pInsert->key = pKey;
    }

    pInsert->val = val;
    return insertAt;
}

int bbMapAdd(bbMapRec* pMap, const bbCHAR* pKey, bbU64PTR val)
{
    bbMapPair* pInsert = bbBSearchGE(pKey, pMap->mpPairs, pMap->mSize, sizeof(bbMapPair), bbCmp_Str2MapPair);
    bbUINT insertAt = pInsert - pMap->mpPairs;

    if ((insertAt >= pMap->mSize) || (bbCmp_Str2MapPair(pKey, pInsert) != 0))
    {
        const bbCHAR* pKeyCopy = bbMapAddKey(pMap, pKey);
        if (!pKeyCopy)
            return -1;

        pInsert = bbMapInsertOne(pMap, insertAt);
        if (!pInsert)
            return -1;

        pInsert->key = pKeyCopy;
    }

    pInsert->val = val;
    return insertAt;
}

bbU64PTR bbMapGet(const bbMapRec* pMap, const bbCHAR* pKey)
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

static void bbMapDerefChunkForKey(bbMapRec* pMap, const bbCHAR* pKey)
{
    struct bbMapKeyChunk** pParent = &pMap->mpKeyChunk;
    struct bbMapKeyChunk* pChunk = pMap->mpKeyChunk;
    while(pChunk)
    {
        if (((bbUPTR)pKey - (bbUPTR)pChunk) < sizeof(struct bbMapKeyChunk))
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

bbU64PTR bbMapDel(bbMapRec* pMap, const bbCHAR* pKey)
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

bbU64PTR bbMapDelIndex(bbMapRec* pMap, bbUINT index)
{
    bbMapPair* pFound = &pMap->mpPairs[index];
    bbU64PTR val = pFound->val;

    bbUINT newSize = pMap->mSize - 1;
    bbUINT capacity = bbMapGetCapcity(newSize);

    bbMapDerefChunkForKey(pMap, pFound->key);

    bbMemMove(pFound, pFound + 1, (newSize - index) * sizeof(bbMapPair));
    bbMemRealloc(capacity * sizeof(bbMapPair), (void**)&pMap->mpPairs);
    pMap->mSize = newSize;

    return val;
}

bbERR bbMapEnumerate(const bbMapRec* pMap, bbERR (*cb)(const bbCHAR*, bbU64PTR, void*), void* user)
{
    bbUINT i = 0;
    for(i=0; i<pMap->mSize; i++)
        if (bbEOK != (*cb)(pMap->mpPairs[i].key, pMap->mpPairs[i].val, user))
            return bbELAST;
    return bbEOK;
}

static bbERR map_copy(const bbCHAR* key, bbU64PTR val, void* pMap)
{
    return bbMapAdd((bbMapRec*)pMap, key, val)<0 ? bbELAST : bbEOK;
}

bbERR bbMapInitCopy(bbMapRec* pDst, const bbMapRec* pSrc)
{
    bbMapInit(pDst);
    if (bbEOK != bbMapEnumerate(pSrc, map_copy, pDst))
    {
        bbMapDestroy(pDst);
        return bbELAST;
    }
    return bbEOK;
}

#ifndef bbDEBUG
void bbMapDump(const bbMapRec* pMap) { pMap=pMap; }
#else
static int bbMapGetChunkNumberForKey(const bbMapRec* pMap, const bbCHAR* pKey)
{
    int i = 0, chunk = -1;
    const struct bbMapKeyChunk* pChunk = pMap->mpKeyChunk;
    while(pChunk)
    {
        if ((bbUPTR)pKey - (bbUPTR)pChunk < sizeof(struct bbMapKeyChunk))
        {
            chunk = i;
            break;
        }
        pChunk = pChunk->next;
        i++;
    }
    return chunk;
}

void bbMapDump(const bbMapRec* pMap)
{
    bbUINT i = 0, keychunks = 0;
    bbU32 datasize = 0, memsize = 0, chunksize;
    const struct bbMapKeyChunk* pChunk;
    const struct bbMapKeyChunk** chunkPtrs;
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

    bbMemFree((void*)chunkEnds);
    bbMemFree((void*)chunkPtrs);
}
#endif

