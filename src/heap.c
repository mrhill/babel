#include "mem.h"
#include "heap.h"
#include "algo.h"

/* 0xFFE0/10 is max index size for PalmOS -> set to 0xFFE0/10/2 */
#define bbHEAPINDEXSIZE 0xCCBU

typedef struct
{
    bbU8* pStart;
    bbU8* pEnd;
    bbU8* pPos;
    bbU32 align16;

} bbHeapChunk;

static bbHeapChunk* gpChunks = NULL;
static bbU8         gChunkActive;
static bbU8         gChunkCount;

bbHeapBlock* bbgpHeapIndex = NULL;

static bbUINT gIndexSize;
static bbUINT gIndexMinFree;
static bbUINT gSubmitNextFree;

bbERR bbHeapInit(bbU32 overallsize, bbUINT chunksize)
{
    bbUINT i;
    long size;

    bbASSERT(!bbgpHeapIndex && !gpChunks);

    /* calc number of chunks needed */

    size = (long) overallsize;
    i = 0; while (size>0) i++, size-=chunksize;

    bbASSERT( (i<256) && (i>0) );

    gChunkActive = 0;
    gChunkCount = (bbU8) i;

    /* claim memory for chunk index */

    if ((gpChunks = (bbHeapChunk*) bbMemAlloc( gChunkCount * sizeof(bbHeapChunk))) == NULL)
    {
        bbHeapDestroy();
        return bbENOMEM;
    }

    bbMemClear( gpChunks, gChunkCount * sizeof(bbHeapChunk));

    /* claim memory for chunks and initialize them */

    for (i=0; i<gChunkCount; ++i)
    {
        if (overallsize > chunksize) size = (long) chunksize; else size = (long) overallsize;
        overallsize -= size;

        if ((gpChunks[i].pStart = gpChunks[i].pPos = (bbU8*) bbMemAlloc(size)) == NULL)
        {
            bbHeapDestroy();
            return bbENOMEM;
        }

        bbASSERT(((bbUPTR)gpChunks[i].pStart & 3) == 0);

        gpChunks[i].pEnd = gpChunks[i].pStart + size;
    }

    /* claim memory for index and intialize it */

    if ((bbgpHeapIndex = (bbHeapBlock*) bbMemAlloc( bbHEAPINDEXSIZE * sizeof(bbHeapBlock))) == NULL)
    {
        bbHeapDestroy();
        return bbENOMEM;
    }

    bbMemClear( bbgpHeapIndex, bbHEAPINDEXSIZE * sizeof(bbHeapBlock));

    for (i=0; i<bbHEAPINDEXSIZE; ++i)
    {
        bbgpHeapIndex[i].size = i + 1; /* next free index */
    }

    gIndexSize    = bbHEAPINDEXSIZE;
    gIndexMinFree = 0;

	return bbEOK;
}

void bbHeapDestroy(void)
{
    bbUINT i;

	bbMemFreeNull( (void**)&bbgpHeapIndex);

    if (gpChunks)
    {
        for (i=0; i<gChunkCount; ++i) bbMemFree(gpChunks[i].pStart);
    }

    bbMemFreeNull( (void**)&gpChunks);
}

static bbHEAPH bbHeapFindFreeIndex(void)
{
    const bbHEAPH freeindex = gIndexMinFree;

    if (freeindex == gIndexSize)
    {
        bbUINT i = freeindex;

        if (bbMemRealloc( (((bbU32)i)<<1) * sizeof(bbHeapBlock), (void**)&bbgpHeapIndex) != bbEOK)
            return bbHEAPH_INVALID;

        gIndexSize = i << 1;

        for (; i<gIndexSize; ++i)
        {
            bbgpHeapIndex[i].ptr = NULL;
            bbgpHeapIndex[i].size = i + 1; /* next free index */
        }
    }

    gSubmitNextFree = bbgpHeapIndex[freeindex].size;

    return freeindex;
}

#define bbHeapSubmitIndex() gIndexMinFree = gSubmitNextFree

/* Compares 2 heap handles by their address */
int bbCmp_bbHEAPH(const void *p1, const void *p2)
{
    void* const adr1 = bbgpHeapIndex[*(bbHEAPH*)p1].ptr;
    void* const adr2 = bbgpHeapIndex[*(bbHEAPH*)p2].ptr;
    #if bbSIZEOF_INT==bbSIZEOF_UPTR
    return (int)adr1-(int)adr2;
    #else
    if (adr1<adr2) return -1;
    if (adr1>adr2) return 1;
    return 0;
    #endif
}

static bbERR bbHeapCompact()
{
    bbHeapBlock* const pHeapIdx = bbgpHeapIndex;
    bbUINT       const indexsize = gIndexSize;
    bbHEAPH* pList, *pListEnd;
    bbUINT   i = 0;
    bbUINT   blockct = 0;

    if ((pList = (bbHEAPH*) bbMemAlloc(gIndexSize*sizeof(bbHEAPH)))==NULL) return bbELAST;

    while (i < indexsize)
    {
        if (pHeapIdx[i].ptr) pList[blockct++] = i;
        i++;
    }

    if (blockct)
    {
        bbQSort(pList, blockct, sizeof(bbHEAPH), bbCmp_bbHEAPH);

        pListEnd = pList + blockct;

        i = 0; // chunk counter
        do
        {
            bbU8*       pDst = gpChunks[i].pStart;
            bbU8* const pEnd = gpChunks[i].pEnd;

            bbASSERT(i < gChunkCount);

            do
            {
                bbUINT blocksize = pHeapIdx[*(pListEnd - blockct)].size;
                blocksize = (blocksize + 3) &~ 3;
                if (blocksize > ((bbUPTR)pEnd - (bbUPTR)pDst))
                    break;

                bbMemMove(pDst, pHeapIdx[*(pListEnd - blockct)].ptr, blocksize);
                pDst += blocksize;
            } while (--blockct);

            gpChunks[i++].pPos = pDst;
        } while (blockct);

        while (i < gChunkCount)
        {
            gpChunks[i].pPos = gpChunks[i].pStart;
            i++;
        }
    }

    bbMemFree(pList);
    return bbEOK;
}

static bbU8* bbHeapFindFreeBlock(bbU32 size)
{
    bbUINT chunk = (bbUINT) gChunkActive;
    bbUINT pass = 2;

    size = (size + 3) &~ 3UL;

    do
    {
        do
        {
            if (((bbUPTR)gpChunks[chunk].pEnd - (bbUPTR)gpChunks[chunk].pPos) >= size)
            {
                bbU8* const pTmp = gpChunks[chunk].pPos;
                gpChunks[chunk].pPos += size;
                return pTmp;
            }

            if (++chunk == gChunkCount) chunk=0;

        } while (chunk != gChunkActive);

        if (bbHeapCompact() != bbEOK)
            return NULL;

    } while(--pass);

    return NULL;
}

bbHEAPH bbHeapAlloc(const bbU32 size)
{
    bbU8* ptr;

    const bbHEAPH heaph = bbHeapFindFreeIndex();

    if ( (heaph != bbHEAPH_INVALID) && 
         ((ptr = bbHeapFindFreeBlock(size)) != NULL) )
    {
        bbgpHeapIndex[heaph].ptr = ptr;
        bbgpHeapIndex[heaph].size = size;

        bbHeapSubmitIndex();

        return heaph;
    }

    bbErrSet(bbENOHEAP);
    return bbHEAPH_INVALID;
}

bbERR bbHeapRealloc(const bbHEAPH heaph, const bbU32 newsize)
{
    if (newsize > bbgpHeapIndex[heaph].size)
    {
        /* xxx do a less greedy implementation */

        bbU8* const ptr = bbHeapFindFreeBlock(newsize);
        if (ptr == NULL)
        {
            return bbErrSet(bbENOHEAP);
        }
        bbMemMove(ptr, bbgpHeapIndex[heaph].ptr, bbgpHeapIndex[heaph].size);
        bbgpHeapIndex[heaph].ptr = ptr;
    }

    bbgpHeapIndex[heaph].size = newsize;

    return bbEOK;
}

void bbHeapFree(const bbHEAPH heaph)
{
    bbgpHeapIndex[heaph].ptr = NULL;

    if (heaph < gIndexMinFree)
    {
        bbgpHeapIndex[heaph].size = gIndexMinFree;
        gIndexMinFree = heaph;
    }
    else
    {
        bbUINT i = heaph;
        do { --i; } while(bbgpHeapIndex[i].ptr); /* xxx this can be slow, do a profiling later */
        bbgpHeapIndex[heaph].size = bbgpHeapIndex[i].size;
        bbgpHeapIndex[i].size = heaph;
    }
}
