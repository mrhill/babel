#include "cpg_unicode.h"
#include "bit.h"
#include "algo.h"

#define bbCPWIDE_BMPINDEX
static bbU8 const gWideBMPIndex[256] = {
    #include "cpg_unicode_iswide.inl"
};
#undef bbCPWIDE_BMPINDEX

#define bbCPWIDE_BMPDATA
static bbU32 const gWideBMPData[] = {
    #include "cpg_unicode_iswide.inl"
};
#undef bbCPWIDE_BMPDATA

int bbCpgUnicode_IsNonspace(bbCHARCP const cp)
{
    return ((cp == 0xFEFF) || (cp == 0xFFFE));//xxx
}

int bbCpgUnicode_IsAlphanum(bbCHARCP const cp)
{
    if ( ((bbU32)(cp - 'a') <= (bbU32)('z'-'a')) ||
         ((bbU32)(cp - 'A') <= (bbU32)('Z'-'A')) ||
         ((bbU32)(cp - '0') <= (bbU32)('9'-'0')) ||
         (cp =='_') )
         return 1;

    return 0;
}

int bbCpgUnicode_IsWide(bbCHARCP const cp)
{
    #if bbSIZEOF_CHARCP >= 2

    if (cp < bbCPWIDE_MIN)
        return 0;

    #if bbSIZEOF_CHARCP > 2
    if (cp < 0x10000UL) /* BMP? */
    #endif
    {
        register const bbU32* p;
        register bbUINT i = (bbUINT) gWideBMPIndex[cp>>8];

        if (i == 0xFF)
            return 0;

        p = &gWideBMPData[i];

        if (i >= bbCPWIDE_IDX_LARGERANGE) /* *p low and high word define range */
            return ((cp >= *(bbU16*)p) && (cp <= *((bbU16*)p+1)));

        #if bbCPUE == bbCPUE_BE
        #error /* *p low and high word define range */
        #endif

        /* p points to 256 bit mask, bit set means wide char */
        i = (bbUINT)cp & 255;
        return bbBitTst(p, i);
    }

    #if bbSIZEOF_CHARCP > 2

    /* include sequence of if-return's */
#define bbCPWIDE_EXT
    #include "cpg_unicode_iswide.inl"
#undef bbCPWIDE_EXT

#if bbCPWIDE_EXTCOUNT > 5
    #error Too many widechar ranges outside BMP, rewrite code
#endif
    #endif /* #if bbSIZEOF_CHARCP > 2 */

    #endif /* #if bbSIZEOF_CHARCP >= 2 */

    return 0;
}


static bbU16 const gBlockIndex[] = { bbCPGUNICODE_BLOCKINDEX };
static bbU32 const gBlockMask[] = { bbCPGUNICODE_BLOCKMASK };

static int bbCmp_UnicodeBlocks(const void* p1, const void* p2)
{
    bbU32 const cp = (bbU32)(bbUPTR)p1 >> 4;
    bbASSERT(cp < 0x110000UL);
    if (cp < (bbUINT)*(bbU16*)p2) return -1;
    if (cp >= (bbUINT)*((bbU16*)p2+1)) return 1;
    return 0;
}

bbUINT bbCpgUnicode_GetBlock(bbCHARCP const cp)
{
    bbUINT blockID;
    bbU16* const pMatch = (bbU16*)bbBSearch((const void*)cp, gBlockIndex, sizeof(gBlockIndex)/sizeof(bbU16)-1, sizeof(bbU16), bbCmp_UnicodeBlocks);

    if (pMatch == NULL)
    {
        #if bbCPGUNICODE_BLOCKINDEX_OOBCOUNT != 2
        #error adjust code
        #endif

        blockID = sizeof(gBlockIndex)/sizeof(bbU16)-1;
        if (cp >= bbCPGUNICODE_BLOCKINDEX_OOB0)
            blockID = sizeof(gBlockIndex)/sizeof(bbU16);
        if (cp >= bbCPGUNICODE_BLOCKINDEX_OOB1)
            blockID = (bbUINT)-1;

        return blockID;
    }

    blockID = ((bbUINT)(bbUPTR)pMatch - (bbUINT)(bbUPTR)&gBlockIndex[0]) >> 1;
    if (bbBitTst(gBlockMask, blockID) == 0)
        blockID = (bbUINT)-1;

    return blockID;
}

bbCHARCP bbCpgUnicode_GetBlockRange(bbUINT const blockID, bbU32* const pSize)
{
    bbU32 start, end;

    if (blockID < sizeof(gBlockIndex)/sizeof(bbU16))
        start = (bbU32) gBlockIndex[blockID] << 4;
    else
        start = bbCPGUNICODE_BLOCKINDEX_OOB0;

    if (blockID < (sizeof(gBlockIndex)/sizeof(bbU16)-1))
        end   = (bbU32) gBlockIndex[blockID+1] << 4;
    else if (blockID == (sizeof(gBlockIndex)/sizeof(bbU16)-1))
        end = bbCPGUNICODE_BLOCKINDEX_OOB0;
    else
        end = bbCPGUNICODE_BLOCKINDEX_OOB1;

    *pSize = end-start;
    return (bbCHARCP) start;
}
