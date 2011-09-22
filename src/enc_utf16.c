#include "str.h"

bbCHARCP bbEncGetCP_utf16le(bbENCSTATE* const pState)
{
    bbCHARCP cp;
    bbUINT   cpsize;
    bbU16*   const pStr = (bbU16*) pState->pStr;

    pState->malformed = 0;
    cpsize = 2;
    cp = (bbCHARCP) bbLD16LE(pStr);
    if bbUTF16_CU_IS_LEAD(cp)
    {
        bbCHARCP const cp2 = (bbCHARCP) bbLD16LE(pStr+1);

        if bbUTF16_CU_IS_TRAIL(cp2)
        {
            cp = bbUTF16_CP_GET_SUPPL(cp, cp2);
            cpsize = 4;
        }
        else
        {
            pState->malformed = (bbU8)cpsize; // flag malformed with !=0
        }
    }

    pState->pStr = (bbU8*)pStr + cpsize;
    pState->cpsize = (bbU8)cpsize;

    return cp;
}

bbCHARCP bbEncGetCP_utf16be(bbENCSTATE* const pState)
{
    bbCHARCP cp;
    bbUINT   cpsize;
    bbU16*   const pStr = (bbU16*) pState->pStr;

    pState->malformed = 0;
    cpsize = 2;
    cp = (bbCHARCP) bbLD16BE(pStr);
    if bbUTF16_CU_IS_LEAD(cp)
    {
        bbCHARCP const cp2 = (bbCHARCP) bbLD16BE(pStr+1);

        if bbUTF16_CU_IS_TRAIL(cp2)
        {
            cp = bbUTF16_CP_GET_SUPPL(cp, cp2);
            cpsize = 4;
        }
        else
        {
            pState->malformed = (bbU8)cpsize; // flag malformed with !=0
        }
    }

    pState->pStr = (bbU8*)pStr + cpsize;
    pState->cpsize = (bbU8)cpsize;

    return cp;
}

void bbEncPutCP_utf16le(bbENCSTATE* const pState, bbCHARCP const cp)
{
    bbU8* pStr = pState->pStr;
    bbUINT w;

    if (((bbCHAR32)(cp)<0x10000UL) && !(bbUTF16_CU_IS_SURROGATE(cp))) {
        bbST16LE(pStr, cp);
        pState->pStr = pStr + (pState->cpsize = 2);
    } else {
        w = ((bbUINT)((cp)>>10)|0xD800U); bbST16LE(pStr, w);
        w = (((bbUINT)(cp)&0x3FFU)|0xDC00U); bbST16LE(pStr+2, w);
        pState->pStr = pStr + (pState->cpsize = 4);
    }
}

void bbEncPutCP_utf16be(bbENCSTATE* const pState, bbCHARCP const cp)
{
    bbU8* pStr = pState->pStr;
    bbUINT w;

    if (((bbCHAR32)(cp)<0x10000UL) && !(bbUTF16_CU_IS_SURROGATE(cp))) {
        bbST16BE(pStr, cp);
        pState->pStr = pStr + (pState->cpsize = 2);
    } else {
        w = ((bbUINT)((cp)>>10)|0xD800U); bbST16BE(pStr, w);
        w = (((bbUINT)(cp)&0x3FFU)|0xDC00U); bbST16BE(pStr+2, w);
        pState->pStr = pStr + (pState->cpsize = 4);
    }
}

