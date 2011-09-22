#include "bit.h"
#include "mem.h"

bbU32 bbBSGetU(bbBSR* const pBS, bbUINT const bitsize)
{
    bbU32 val;
    bbBSRGETU(*pBS, val, bitsize)
    return val;
}

bbS32 bbBSGetS(bbBSR* const pBS, bbUINT const bitsize)
{
    bbS32 val;
    bbBSRGETS(*pBS, val, bitsize)
    return val;
}

bbUINT bbBSRGet1(bbBSR* const pBS)
{
    bbUINT val;
    bbBSRGET1(*pBS, val)
    return val;
}

void bbBSRSkipFar(bbBSR* const pBS, bbU32 const bitsize)
{
    bbBSRSKIPFAR(*pBS, bitsize)
}

void bbBSWPut(bbBSW* const pBS, bbU32 const val, bbUINT const bitsize)
{
    bbBSWPUT(*pBS, val, bitsize)
}

void bbBSWPut1(bbBSW* const pBS, bbUINT const val)
{
    bbBSWPUT1(*pBS, val)
}

void bbBSWSkip(bbBSW* const pBS, bbU32 const bitsize)
{
    bbBSWSKIP(*pBS, bitsize);
}

/** Reallocate memory for bitstream container.
    @param pBS Bitstream container
    @param newsize New size in bytes
*/
static bbERR bbBSRealloc(bbBS* const pBS, const bbU32 newsize)
{
    bbU32* const pBufOld = pBS->pBuf;

    bbERR const err = bbMemRealloc( newsize, (void**) &pBS->pBuf);
    if (err != bbEOK)
    {
        return err;
    }
    else
    {
        bbS32 const diff = (bbS32)(bbUPTR)pBS->pBuf - (bbS32)(bbUPTR)pBufOld;
        pBS->pBufEnd = (bbU32*) ((bbU8*)pBS->pBuf + newsize);
        pBS->bsr.ptr = (bbU32*) ((bbU8*)pBS->bsr.ptr + diff);
        pBS->bsw.ptr = (bbU32*) ((bbU8*)pBS->bsw.ptr + diff);
    }

    return bbEOK;
}

void bbBSInit(bbBS* const pBS)
{
    bbMemClear( pBS, sizeof(bbBS));
}

void bbBSDestroy(bbBS* const pBS)
{
    bbMemFreeNull( (void**)&pBS->pBuf);
}

bbERR bbBSPut(bbBS* const pBS, bbU32 const val, bbUINT const bitsize)
{
    if ( (pBS->bsw.ptr + ((pBS->bsw.pos + bitsize)>>3)) >= pBS->pBufEnd)
    {
        bbERR err;
        bbU32 size = (bbU32)(bbUPTR)pBS->pBufEnd - (bbU32)(bbUPTR)pBS->pBuf;
        if (size == 0) size = 8;
        err = bbBSRealloc(pBS, size<<1);
        if (err != bbEOK) return err;
    }

    bbBSWPUT(pBS->bsw, val, bitsize)

    return bbEOK;
}

void bbBSPutUnsafe(bbBS* const pBS, bbU32 const val, bbUINT const bitsize)
{
    bbBSWPUT(pBS->bsw, val, bitsize)
}

bbERR bbBSEnsure(bbBS* const pBS, bbU32 bitsize)
{
    if ( (pBS->bsw.ptr + ((pBS->bsw.pos + bitsize)>>3)) < pBS->pBufEnd)
    {
        return bbEOK;
    }
    else
    {
        bbU32 const pos = bbBSGetPosW(pBS);

        bbU32 size = (bbU32)(bbUPTR)pBS->pBufEnd - (bbU32)(bbUPTR)pBS->pBuf;
        if (size == 0) size = 8;

        bitsize += pos;
        do { size <<= 1; } while (bitsize > (size<<3));

        return bbBSRealloc(pBS, size);
    }
}

void bbBSSeekR(bbBS* pBS, bbU32 pos)
{
    bbASSERT((pBS->pBuf + (pos>>5) + ((pos&31)?1:0)) <= pBS->pBufEnd);

    pBS->bsr.ptr = pBS->pBuf + (pos >> 5);
    pBS->bsr.pos = 0;
    pBS->bsr.cache = 0;

    if ((pos = pos & 31) != 0)
    {
        bbBSRSKIP(pBS->bsr, pos)
    }
}

void bbBSSeekW(bbBS* pBS, bbU32 pos)
{
    bbASSERT((pBS->pBuf + (pos>>5) + ((pos&31)?1:0)) <= pBS->pBufEnd);

    pBS->bsw.ptr = pBS->pBuf + (pos >> 5);
    pBS->bsw.pos = pos & 31;
}

