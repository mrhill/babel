#include "str.h"

const bbU8 bbgUTF8_CountTrailBytes[32] = { 
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0xxxx.xxx, 0 - 0x7F
    0,0,0,0,0,0,0,0,                 // 10xxx.xxx, trail bytes, invalid as lead
    1,1,1,1,                         // 110xx.xxx, 0 (0x80) - 0x7FF
    2,2,                             // 1110x.xxx, 0 (0x800) - 0xFFFF
    3,                               // 11110.xxx, 0 (0x10000) - 0x13FFFF (F5,F6,F7 invalid), 0x10FFFF is last UNICODE char)
    0                                // 11111.xxx, invalid lead
};

const bbU8 bbgUTF8_LeadMask[4] = { 63, 31, 15, 7 };

bbCHARCP bbEncGetCP_utf8(bbENCSTATE* const pState)
{
    bbU8*    pStr = pState->pStr;
    bbCHARCP cp = (bbCHARCP) *(pStr++);
    bbCHARCP cp1, cp2;
    bbUINT   count, malformed;

    if (cp < 0x80)
    {
        pState->pStr      = pStr;
        pState->cpsize    = 1;
        pState->malformed = 0;
        return cp;
    }

    count = (bbUINT) bbgUTF8_CountTrailBytes[(bbUINT)cp >> 3];
    cp2 = cp & (bbUINT)bbgUTF8_LeadMask[count];

    malformed = 1;

    switch(count)
    {
    case 3:
        if (cp >= 0xF5) // F5, F6, F7 are invalid leads
            break;
        cp1 = (bbCHARCP) *(pStr++);
        if ((cp1 & 0xC0U) != 0x80) break; // verify if trail
        cp2 = (cp2 << 6) | (cp1 & 0x3FU);
    case 2:            
        cp1 = (bbCHARCP) *(pStr++);
        if ((cp1 & 0xC0U) != 0x80) break; // verify if trail
        cp2 = (cp2 << 6) | (cp1 & 0x3FU);
    case 1:
        cp1 = (bbCHARCP) *pStr;
        if ((cp1 & 0xC0U) != 0x80) break; // verify if trail
        cp = (cp2 << 6) | (cp1 & 0x3FU);

        malformed = 0;
        break;

    // case 0 is trail or invalid lead
    }

    if (malformed) count = 0;
    ++count;
        
    pState->malformed = (bbU8)malformed;
    pState->cpsize    = (bbU8)count;
    pState->pStr     += count;
    return cp;
}

void bbEncPutCP_utf8(bbENCSTATE* const pState, bbCHARCP const cp)
{
    bbU8* pStr = pState->pStr;

    if (cp < 0x80)
    {
        *pStr++ = (bbU8)cp;
    }
    else if (cp < 0x800)
    {
        *pStr++ = (bbU8)((cp >> 6) | 0xC0);
        *pStr++ = (bbU8)((cp & 63) | 0x80);
    }
    else if (cp < 0x10000)
    {
        *pStr++ = (bbU8)((cp >> 12) | 0xE0);
        *pStr++ = (bbU8)(((cp>>6)&63) | 0x80);
        *pStr++ = (bbU8)((cp & 63) | 0x80);
    }
    else if (cp < 0x140000)
    {
        *pStr++ = (bbU8)((cp >> 18) | 0xF0);
        *pStr++ = (bbU8)(((cp>>12)&63) | 0x80);
        *pStr++ = (bbU8)(((cp>>6)&63) | 0x80);
        *pStr++ = (bbU8)((cp & 63) | 0x80);
    }

    pState->cpsize = (bbUINT)(bbUPTR)pStr - (bbUINT)(bbUPTR)pState->pStr;
    pState->pStr = pStr;
}

