#include "enc.h"
#include "mem.h"

bbCHARCP bbEncGetCP_sbc(bbENCSTATE* const pState)
{
    pState->cpsize = 1;
    return (bbCHARCP) *(pState->pStr++);
}

bbCHARCP bbEncGetCUBack_sbc(bbENCSTATE* const pState)
{
    pState->cpsize = 1;
    return (bbCHARCP) *(--pState->pStr);
}

static void bbEncPutCP_sbc(bbENCSTATE* const pState, bbCHARCP const cp)
{
    pState->cpsize = 1;
    *(pState->pStr++) = (bbU8)cp;
}

static void bbEncInit_null(bbENCSTATE* const pState)
{
    bbMemClear(pState, sizeof(bbENCSTATE));
}

static bbCHARCP bbEncGetCP_dbcle(bbENCSTATE* const pState)
{
    bbUINT const c = bbLD16LE(pState->pStr);
    pState->pStr += 2;
    pState->cpsize = 2;
    return (bbCHARCP)c;
}

static bbCHARCP bbEncGetCUBack_dbcle(bbENCSTATE* const pState)
{
    pState->pStr -= 2;
    pState->cpsize = 2;
    return (bbCHARCP)bbLD16LE(pState->pStr);
}

static void bbEncPutCP_dbcle(bbENCSTATE* const pState, bbCHARCP const cp)
{
    bbST16LE(pState->pStr, cp);
    pState->pStr += 2;
    pState->cpsize = 2;
}

static bbCHARCP bbEncGetCP_dbcbe(bbENCSTATE* const pState)
{
    bbUINT const c = bbLD16BE(pState->pStr);
    pState->pStr += 2;
    pState->cpsize = 2;
    return (bbCHARCP)c;
}

static bbCHARCP bbEncGetCUBack_dbcbe(bbENCSTATE* const pState)
{
    pState->pStr -= 2;
    pState->cpsize = 2;
    return (bbCHARCP)bbLD16BE(pState->pStr);
}

static void bbEncPutCP_dbcbe(bbENCSTATE* const pState, bbCHARCP const cp)
{
    bbST16BE(pState->pStr, cp);
    pState->pStr += 2;
    pState->cpsize = 2;
}

static bbCHARCP bbEncGetCP_utf32le(bbENCSTATE* const pState)
{
    bbU32 const c = bbLD32LE(pState->pStr);
    pState->pStr += 4;
    pState->cpsize = 4;
    pState->malformed = (((c-0xD800U) < (0xE000U-0xD800U)) || (c >= 0x00110000UL)) ? 1 : 0;
    return (bbCHARCP)(c & ((1UL<<22)-1));
}

static bbCHARCP bbEncGetCUBack_utf32le(bbENCSTATE* const pState)
{
    bbU32 c;
    pState->pStr -= 4;
    pState->cpsize = 4;
    c = bbLD32LE(pState->pStr);
    pState->malformed = (((c-0xD800U) < (0xE000U-0xD800U)) || (c >= 0x00110000UL)) ? 1 : 0;
    return (bbCHARCP)(c & ((1UL<<22)-1));
}

static bbCHARCP bbEncGetCP_utf32be(bbENCSTATE* const pState)
{
    bbU32 const c = bbLD32BE(pState->pStr);
    pState->pStr += 4;
    pState->cpsize = 4;
    pState->malformed = (((c-0xD800U) < (0xE000U-0xD800U)) || (c >= 0x00110000UL)) ? 1 : 0;
    return (bbCHARCP)(c & ((1UL<<22)-1));
}

static bbCHARCP bbEncGetCUBack_utf32be(bbENCSTATE* const pState)
{
    bbU32 c;
    pState->cpsize = 4;
    pState->pStr -= 4;
    c = bbLD32BE(pState->pStr);
    pState->malformed = (((c-0xD800U) < (0xE000U-0xD800U)) || (c >= 0x00110000UL)) ? 1 : 0;
    bbASSERT(pState->cpsize == 4);
    return (bbCHARCP)(c & ((1UL<<22)-1));
}

static void bbEncPutCP_utf32le(bbENCSTATE* const pState, bbCHARCP const cp)
{
    bbST32LE(pState->pStr, cp);
    pState->pStr += 4;
    pState->cpsize = 4;
}

static void bbEncPutCP_utf32be(bbENCSTATE* const pState, bbCHARCP const cp)
{
    bbST32BE(pState->pStr, cp);
    pState->pStr += 4;
    pState->cpsize = 4;
}

static const bbU8 gChr2Base64[0x7B-0x2B] = // ASCII 0x2B..0x7A -> BASE64 lookup
{
    0x3E,0xFF,0xFF,0xFF,0x3F,
    0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
    0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33
};

static const bbU8 gChr2UTF7Type[128/4] = // 2 bit array, 0=invalid, 1=base64, 2=safe, 3=optional
{
    0x00,0x00,0x28,0x08,0x00,0x00,0x00,0x00,0xFC,0xB0,0x78,0x68,0x54,0x54,0xE4,0xBC,
    0x54,0x54,0x54,0x54,0x54,0x54,0xD4,0xFC,0x54,0x54,0x54,0x54,0x54,0x54,0xD4,0x0C
};

static bbCHARCP bbEncGetCP_utf7(bbENCSTATE* const pState)
{
    bbU8*  pStr  = pState->pStr;
    bbU32  bits  = pState->state;
    bbUINT nbits = bits & 0x1F; // 5 bits bitpos % 16 in BASE64 string
    bbUINT state = bits & 0x20; // 1 bit  set = BASE64 active
    bbU32  c, b;
    bits >>= 6;                 // cached bits of BASE64 string

    pState->malformed = 0;

    for (;;)
    {
        c = *(pStr++);

        if (!state)
        {
            if (c != '+')
                break;
            c = *(pStr++);
            if (c == '-')
            {
                c = '+';
                break;
            }
            state = 0x20;
            nbits = 0;
        }

        while (b=c-0x2B, (b < (0x7B-0x2B)) && ((b=gChr2Base64[b]) != 0xFF))
        {
            bits = (bits << 6) | b;
            if ((nbits+=6) >= 16)
            {
                nbits -= 16;
                c = (bits >> nbits) & 0xFFFFU;
                goto bbEncGetCP_utf7_out;
            }
            c = *(pStr++);
        }
        state = 0;
        if (c == '-')
            continue;
        break;
    }

    if ((c>=128) || ((gChr2UTF7Type[c>>2]>>(c&3)) == 0))
        pState->malformed = 1;

    bbEncGetCP_utf7_out:

    pState->state = (bits<<6) | state | nbits;
    pState->cpsize = (bbUINT)(bbUPTR)pStr - (bbUINT)(bbUPTR)pState->pStr;
    pState->pStr = pStr;

    return (bbCHARCP)c;
}

static void bbEncPutCP_utf7(bbENCSTATE* const pState, bbCHARCP const cp)
{
    bbST16LE(pState->pStr, cp);
    pState->pStr += 2;
}

static void bbEncInit_utf32(bbENCSTATE* const pState)
{
    bbMemClear(pState, sizeof(bbENCSTATE));
    pState->cpsize = 4;
}


static const bbENCTABLE gEncTabLookUp[] =
{
    #if bbENC_SBC != 0
    #error
    #endif
    {
        bbEncGetCP_sbc,
        bbEncGetCP_sbc,
        bbEncGetCUBack_sbc,
        bbEncPutCP_sbc,
        bbEncInit_null,
        bbT("SBC"),
        bbT("Single byte encoding"),
        1,
        1,
        1,
        0
    },

    #if bbENC_UTF8 != 1
    #error
    #endif
    {
        bbEncGetCP_utf8,
        bbEncGetCP_sbc,
        bbEncGetCUBack_sbc,
        bbEncPutCP_utf8,
        bbEncInit_null,
        bbT("UTF-8"),
        bbT("8-bit unicode encoding"),
        1,
        bbUTF8_CP_MAXCU,
        1,
        0
    },

    #if bbENC_DBCLE != 2
    #error
    #endif
    {
        bbEncGetCP_dbcle,
        bbEncGetCP_dbcle,
        bbEncGetCUBack_dbcle,
        bbEncPutCP_dbcle,
        bbEncInit_null,
        bbT("DBC LE"),
        bbT("Double byte encoding, little endian"),
        2,
        2,
        1,
        0
    },

    #if bbENC_DBCBE != 3
    #error
    #endif
    {
        bbEncGetCP_dbcbe,
        bbEncGetCP_dbcbe,
        bbEncGetCUBack_dbcbe,
        bbEncPutCP_dbcbe,
        bbEncInit_null,
        bbT("DBC BE"),
        bbT("Double byte encoding, big endian"),
        2,
        2,
        1,
        0
    },

    #if bbENC_UTF16LE != 4
    #error
    #endif
    {
        bbEncGetCP_utf16le,
        bbEncGetCP_dbcle,
        bbEncGetCUBack_dbcle,
        bbEncPutCP_utf16le,
        bbEncInit_null,
        bbT("UTF-16LE"),
        bbT("16-bit unicode encoding, little endian"),
        2,
        bbUTF16_CP_MAXCU * 2,
        1,
        5
    },

    #if bbENC_UTF16BE != 5
    #error
    #endif
    {
        bbEncGetCP_utf16be,
        bbEncGetCP_dbcbe,
        bbEncGetCUBack_dbcbe,
        bbEncPutCP_utf16be,
        bbEncInit_null,
        bbT("UTF-16BE"),
        bbT("16-bit unicode encoding, big endian"),
        2,
        bbUTF16_CP_MAXCU * 2,
        1,
        5
    },

    #if bbENC_UTF32LE != 6
    #error
    #endif
    {
        bbEncGetCP_utf32le,
        bbEncGetCP_utf32le,
        bbEncGetCUBack_utf32le,
        bbEncPutCP_utf32le,
        bbEncInit_utf32,
        bbT("UTF-32LE"),
        bbT("4-Byte unicode encoding, little endian"),
        4,
        4,
        1,
        4
    },

    #if bbENC_UTF32BE != 7
    #error
    #endif
    {
        bbEncGetCP_utf32be,
        bbEncGetCP_utf32be,
        bbEncGetCUBack_utf32be,
        bbEncPutCP_utf32be,
        bbEncInit_utf32,
        bbT("UTF-32BE"),
        bbT("4-Byte unicode encoding, big endian"),
        4,
        4,
        1,
        4
    },

    #if bbENC_UTF7 != 8
    #error
    #endif
    {
        bbEncGetCP_utf7,
        bbEncGetCP_sbc,
        bbEncGetCUBack_sbc,
        bbEncPutCP_utf7,
        bbEncInit_null,
        bbT("UTF-7"),
        NULL,
        1,
        8,//xxx
        0,
        4
    },

    #if bbENC_SCSU != 9
    #error
    #endif
    {
        NULL,
        bbEncGetCP_sbc,
        bbEncGetCUBack_sbc,
        NULL,
        bbEncInit_null,
        bbT("SCSU"),
        NULL,
        1,
        8,//xxx
        0,
        0
    }
};

bbENCTABLE* bbEncGetTable(bbUINT const enc)
{
    if (enc < (sizeof(gEncTabLookUp) / sizeof(bbENCTABLE)))
        return (bbENCTABLE*) gEncTabLookUp + enc;

    bbErrSet(bbEBADPARAM);
    return NULL;
}

