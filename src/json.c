#include "json.h"
#include "fixmath.h"
#include "file.h"
#include <math.h>

void bbJsonValInit(bbJsonVal* pVal)
{
    bbMemClear(pVal, sizeof(bbJsonVal));
}

void bbJsonValInitType(bbJsonVal* pVal, bbJSONTYPE type)
{
    bbMemClear(pVal, sizeof(bbJsonVal));
    pVal->mType = type;
}

void bbJsonValDestroy(bbJsonVal* pVal)
{
    bbUINT i;

    if (!pVal)
       return;

    switch (pVal->mType)
    {
    case bbJSONTYPE_ARRAY:
        for (i=0; i<pVal->u.array.length; i++)
            bbJsonValDestroy(pVal->u.array.values + i);

        bbMemFree(pVal->u.array.values);
        break;

    case bbJSONTYPE_OBJECT:
        for (i=0; i<bbMapGetSize(&pVal->u.object); i++)
        {
            bbJsonVal* pChild = (bbJsonVal*)(bbUPTR)(bbMapGetPair(&pVal->u.object, i)->val);
            bbJsonValDestroy(pChild);
            bbMemFree(pChild);
        }

        bbMapDestroy(&pVal->u.object);
        break;

    case bbJSONTYPE_STRING:
        bbMemFree(pVal->u.string.ptr);
        break;

    default:
        break;
    }

    bbMemClear(pVal, sizeof(bbJsonVal));
}

bbERR bbJsonValAssign(bbJsonVal* pVal, const bbJsonVal* pOther)
{
    bbERR err = bbEOK;
    if (!pVal)
        err = bbErrSet(bbEBADPARAM);
    else
    {
        bbJsonValDestroy(pVal);
        if (pOther)
            err = bbJsonValInitCopy(pVal, pOther);
    }
    return err;
}

bbJsonVal* bbJsonIntAssign(bbJsonVal* pVal, bbS64 v)
{
    if (pVal)
    {
        bbJsonValClear(pVal);
        pVal->mType = bbJSONTYPE_INTEGER;
        pVal->u.integer = v;
    }
    return pVal;
}

bbJsonVal* bbJsonStrAssign(bbJsonVal* pVal, const bbCHAR* str)
{
    if (pVal)
    {
        bbJsonValClear(pVal);
        pVal->mType = bbJSONTYPE_STRING;
        if (str)
        {
            if (!(pVal->u.string.ptr = bbStrDup(str)))
                return NULL;
            pVal->u.string.length = bbStrLen(str);
        }
    }
    return pVal;
}

bbERR bbJsonValDump(const bbJsonVal* v, bbStrBuf* s, bbUINT indent)
{
    bbUINT i, j;

    switch (v->mType)
    {
    case bbJSONTYPE_NONE:
        bbStrBufCatCStr(s, "none");
        break;

    case bbJSONTYPE_OBJECT:
        bbStrBufCatCP(s, '{');
        for (i=0; i<bbMapGetSize(&v->u.object); ++i)
        {
            if (i) bbStrBufCatCP(s, ',');
            if (indent)
            {
                bbStrBufCatCP(s, '\n');
                for(j=0; j<indent; ++j)
                    bbStrBufCatCP(s, '\t');
            }
            bbStrBufCatf(s, bbT("\"%s\":"), bbMapGetPair(&v->u.object, i)->key);
            if (bbEOK != bbJsonValDump((bbJsonVal*)(bbUPTR)bbMapGetPair(&v->u.object, i)->val, s, indent?indent+1:0))
                return bbELAST;
        }
        if (indent)
        {
            bbStrBufCatCP(s, '\n');
            for(j=1; j<indent; ++j)
                bbStrBufCatCP(s, '\t');
        }
        bbStrBufCatCP(s, '}');
        break;

    case bbJSONTYPE_ARRAY:
        bbStrBufCatCP(s, '[');
        for (i=0; i<v->u.array.length; ++i)
        {
            if (i) bbStrBufCatCP(s, ',');
            if (bbEOK != bbJsonValDump(v->u.array.values + i, s, indent))
                return bbELAST;
        }
        bbStrBufCatCP(s, ']');
        break;

    case bbJSONTYPE_INTEGER:
        bbStrBufCatf(s, bbT("%ld"), v->u.integer);
        break;

    case bbJSONTYPE_DOUBLE:
        bbStrBufCatf(s, bbT("%lf"), v->u.dbl);
        break;

    case bbJSONTYPE_STRING:
        bbStrBufCatf(s, bbT("\"%s\""), v->u.string.ptr);
        break;

    case bbJSONTYPE_BOOLEAN:
        bbStrBufCatCStr(s, v->u.boolean ? "true" : "false");
        break;

    case bbJSONTYPE_NULL:
        bbStrBufCatCStr(s, "null");
        break;

    default:
        return bbErrSet(bbEBADPARAM);
    }
    return bbEOK;
}

bbERR bbJsonValSave(const bbJsonVal* v, const bbCHAR* pFile, bbUINT indent)
{
    bbERR err = bbELAST;
    bbFILEH fh = bbFileOpen(pFile, bbFILEOPEN_READWRITE|bbFILEOPEN_TRUNC);
    if (fh)
    {
        bbStrBuf str;
        bbStrBufInit(&str);

        if (bbEOK == (err = bbJsonValDump(v, &str, indent)))
        {
            err = bbFileWrite(fh, bbStrBufGetPtr(&str), bbStrBufGetLen(&str));
        }

        bbStrBufDestroy(&str);
        bbFileClose(fh);
    }
    return err;
}

bbERR bbJsonValLoad(bbJsonVal* pVal, const bbCHAR* pFile)
{
    bbERR err = bbELAST;
    bbCHAR* json;
    bbFILEH fh = bbFileOpen(pFile, bbFILEOPEN_READ);
    bbStrBuf str;
    bbStrBufInit(&str);

    if (!fh)
        goto err;

    json = bbStrBufSetLen(&str, bbFileExt(fh));
    if (!json)
        goto err;

    if (bbEOK != bbFileRead(fh, json, bbStrBufGetLen(&str)))
        goto err;

    json[bbStrBufGetLen(&str)] = 0; // 0-terminate

    bbJsonValDestroy(pVal);
    err = bbJsonValInitParse(pVal, json, bbStrBufGetLen(&str));

    err:
    bbStrBufDestroy(&str);
    bbFileClose(fh);
    return err;
}

static bbJsonVal* bbJsonValCopy(const bbJsonVal* pVal)
{
    bbJsonVal* pNew = bbMemAlloc(sizeof(bbJsonVal));
    if (pNew)
    {
        if (bbEOK != bbJsonValInitCopy(pNew, pVal))
            bbMemFreeNull((void**)&pNew);
    }
    return pNew;
}

static bbERR bbJsonValCopyMapCB(const bbCHAR* key, bbU64PTR val, void* pMap)
{
    bbJsonVal* pValCopy = bbJsonValCopy((const bbJsonVal*)(bbUPTR)val);

    if (!pValCopy || (bbMapAdd((bbMapRec*)pMap, key, (bbUPTR)pValCopy) < 0))
    {
        bbJsonValDestroy(pValCopy);
        bbMemFree(pValCopy);
        return bbELAST;
    }

    return bbEOK;
}

static bbERR bbJsonValCopyMap(bbJsonVal* pNew, const bbJsonVal* pVal)
{
    bbMapInit(&pNew->u.object);
    if (bbEOK != bbMapEnumerate(&pVal->u.object, bbJsonValCopyMapCB, &pNew->u.object))
    {
        bbJsonValDestroy(pNew);
        return bbELAST;
    }
    return bbEOK;
}

bbERR bbJsonValInitCopy(bbJsonVal* pNew, const bbJsonVal* pVal)
{
    bbUINT i;

    bbJsonValInit(pNew);
    if (!pVal)
        return bbEOK;
    pNew->mType = pVal->mType;

    switch(pVal->mType)
    {
    case bbJSONTYPE_OBJECT:
        if (bbEOK != bbJsonValCopyMap(pNew, pVal))
            return bbELAST;
        break;

    case bbJSONTYPE_ARRAY:
        pNew->u.array.values = bbMemAlloc(pVal->u.array.length * sizeof(struct bbJsonVal));
        if (!pNew->u.array.values)
            return bbELAST;
        pNew->u.array.length = pVal->u.array.length;
        for (i=0; i<pVal->u.array.length; i++)
        {
            if (bbEOK != bbJsonValInitCopy(pNew->u.array.values + i, pVal->u.array.values + i))
            {
                while(i--)
                    bbJsonValDestroy(pNew->u.array.values + i);
                bbMemFreeNull((void**)&pNew->u.array.values);
                return bbELAST;
            }
        }
        break;

    case bbJSONTYPE_STRING:
        if (pVal->u.string.ptr)
        {
            pNew->u.string.ptr = bbStrDup(pVal->u.string.ptr);
            if (!pNew->u.string.ptr && pVal->u.string.length)
                return bbELAST;
            pNew->u.string.length = pVal->u.string.length;
        }
        break;

    default:
        pNew->u.integer = pVal->u.integer;
        break;
    }

    return bbEOK;
}

bbUINT bbJsonObjGetSize(const bbJsonVal* pVal)
{
    return pVal && pVal->mType == bbJSONTYPE_OBJECT ? bbMapGetSize(&(pVal)->u.object) : 0;
}

bbJsonVal* bbJsonObjAdd(bbJsonVal* pVal, const bbCHAR* key, const bbJsonVal* pObj)
{
    bbJsonVal* pNew;

    if (pVal->mType == bbJSONTYPE_NONE)
        pVal->mType = bbJSONTYPE_OBJECT;

    if (pVal->mType != bbJSONTYPE_OBJECT)
    {
        bbErrSet(bbEBADPARAM);
        return NULL;
    }

    if (!(pNew = bbJsonValCopy(pObj)))
        return NULL;

    if (bbMapAdd(&pVal->u.object, key, (bbUPTR)pNew) < 0)
    {
        bbJsonValDestroy(pNew);
        return NULL;
    }

    return pNew;
}

bbJsonVal* bbJsonObjAddStr(bbJsonVal* pVal, const bbCHAR* key, const bbCHAR* str)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_STRING);
    v.u.string.ptr = (bbCHAR*)str;
    return bbJsonObjAdd(pVal, key, &v);
}

bbJsonVal* bbJsonObjAddInt(bbJsonVal* pVal, const bbCHAR* key, bbS64 n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_INTEGER);
    v.u.integer = n;
    return bbJsonObjAdd(pVal, key, &v);
}

bbJsonVal* bbJsonObjAddDbl(bbJsonVal* pVal, const bbCHAR* key, double n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_DOUBLE);
    v.u.dbl = n;
    return bbJsonObjAdd(pVal, key, &v);
}

bbJsonVal* bbJsonObjAddBool(bbJsonVal* pVal, const bbCHAR* key, int n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_BOOLEAN);
    v.u.boolean = n;
    return bbJsonObjAdd(pVal, key, &v);
}

void bbJsonObjDel(bbJsonVal* pVal, const bbCHAR* key)
{
    bbJsonVal* v;

    if (pVal->mType != bbJSONTYPE_OBJECT)
        return;

    v = (bbJsonVal*)(bbUPTR)bbMapDel(&pVal->u.object, key);
    bbJsonValDestroy(v);
    bbMemFree(v);
}

bbS64 bbJsonObjGetInt(const bbJsonVal* pVal, const bbCHAR* key, bbS64 dflt)
{
    bbJsonVal* v = bbJsonObjGet(pVal, key);
    if (!v)
        return dflt;

    switch(v->mType)
    {
    case bbJSONTYPE_INTEGER:
        return v->u.integer;
    case bbJSONTYPE_DOUBLE:
        return (bbS64)v->u.dbl;
    case bbJSONTYPE_STRING:
        {
        bbS64 num;
        if (v->u.string.ptr && (bbEOK == bbStrToS64(v->u.string.ptr, NULL, &num, bbSTROPT_ALLFMT)))
            return num;
        else
            return dflt;
        }
    case bbJSONTYPE_BOOLEAN:
        return v->u.boolean;
    default:
        return dflt;
    }
}

int bbJsonObjGetBool(const bbJsonVal* pVal, const bbCHAR* key, int dflt)
{
    bbJsonVal* v = bbJsonObjGet(pVal, key);
    if (!v)
        return dflt;
    if (v->mType == bbJSONTYPE_STRING && v->u.string.ptr)
    {
        if (!bbStrICmp(v->u.string.ptr, "TRUE"))
            return !0;
        if (!bbStrICmp(v->u.string.ptr, "FALSE"))
            return 0;
    }
    return bbJsonObjGetInt(pVal, key, dflt) != 0;
}

const bbCHAR* bbJsonObjGetStr(const bbJsonVal* pVal, const bbCHAR* key, const bbCHAR* dflt)
{
    bbJsonVal* v = bbJsonObjGet(pVal, key);
    if (v && v->mType==bbJSONTYPE_STRING)
        return v->u.string.ptr;
    else
        return dflt;
}

bbJsonVal* bbJsonObjEnsure(bbJsonVal* pVal, const bbCHAR* key)
{
    bbJsonVal* v = bbJsonObjGet(pVal, key);
    return v ? v : bbJsonObjAdd(pVal, key, NULL);
}

bbERR bbJsonObjMerge(bbJsonVal* pVal, const bbJsonVal* pOther)
{
    if (pVal && pVal->mType == bbJSONTYPE_NONE)
        pVal->mType = bbJSONTYPE_OBJECT;

    if ((pVal && pVal->mType != bbJSONTYPE_OBJECT) ||
        (pOther && pOther->mType != bbJSONTYPE_OBJECT))
        return bbErrSet(bbEBADPARAM);

    if (pVal && pOther)
        return bbMapEnumerate(&pOther->u.object, bbJsonValCopyMapCB, &pVal->u.object);

    return bbEOK;
}

#define bbJsonArrGetCapacity(size) ((size) ? 2 << bbGetTopBit((size)-1) : 0)

bbUINT bbJsonArrGetSize(const bbJsonVal* pVal)
{
    return pVal && pVal->mType == bbJSONTYPE_ARRAY ? pVal->u.array.length : 0;
}

bbJsonVal* bbJsonArrIns(bbJsonVal* pVal, int pos, const bbJsonVal* pObj, bbUINT count)
{
    bbJsonVal* pInsert;
    bbUINT i, oldCapacity, newCapacity;

    if (!pVal || pVal->mType != bbJSONTYPE_ARRAY)
    {
        if (pVal && pVal->mType == bbJSONTYPE_NONE)
            pVal->mType = bbJSONTYPE_ARRAY;
        else
        {
            bbErrSet(bbEBADPARAM);
            return NULL;
        }
    }

    if ((bbUINT)pos > pVal->u.array.length)
        pos = pVal->u.array.length;

    oldCapacity = bbJsonArrGetCapacity(pVal->u.array.length);
    newCapacity = bbJsonArrGetCapacity(pVal->u.array.length + count);
    if ((oldCapacity != newCapacity) && (bbEOK != bbMemRealloc(newCapacity * sizeof(struct bbJsonVal), (void**)&pVal->u.array.values)))
        return NULL;

    pInsert = pVal->u.array.values + pos;
    bbMemMove(pInsert + count, pInsert, (pVal->u.array.length - pos) * sizeof(bbJsonVal));
    for(i=0; i<count; i++)
        bbJsonValInitCopy(pInsert + i, pObj ? pObj + i : NULL);

    pVal->u.array.length += count;

    return pInsert;
}

bbJsonVal* bbJsonArrInsArrU8(bbJsonVal* pVal, int pos, const bbU8* pArr, bbUINT count)
{
    bbJsonVal* pInsert = bbJsonArrIns(pVal, pos, NULL, count);
    if (pInsert)
        while(count--)
        {
            pInsert[count].u.integer = pArr[count];
            pInsert[count].mType = bbJSONTYPE_INTEGER;
        }
    return pInsert;
}

bbJsonVal* bbJsonArrInsArrU16(bbJsonVal* pVal, int pos, const bbU16* pArr, bbUINT count)
{
    bbJsonVal* pInsert = bbJsonArrIns(pVal, pos, NULL, count);
    if (pInsert)
        while(count--)
        {
            pInsert[count].u.integer = pArr[count];
            pInsert[count].mType = bbJSONTYPE_INTEGER;
        }
    return pInsert;
}

bbJsonVal* bbJsonArrInsArrU32(bbJsonVal* pVal, int pos, const bbU32* pArr, bbUINT count)
{
    bbJsonVal* pInsert = bbJsonArrIns(pVal, pos, NULL, count);
    if (pInsert)
        while(count--)
        {
            pInsert[count].u.integer = pArr[count];
            pInsert[count].mType = bbJSONTYPE_INTEGER;
        }
    return pInsert;
}

bbJsonVal* bbJsonArrInsArrU64(bbJsonVal* pVal, int pos, const bbU64* pArr, bbUINT count)
{
    bbJsonVal* pInsert = bbJsonArrIns(pVal, pos, NULL, count);
    if (pInsert)
        while(count--)
        {
            pInsert[count].u.integer = pArr[count];
            pInsert[count].mType = bbJSONTYPE_INTEGER;
        }
    return pInsert;
}

bbJsonVal* bbJsonArrInsStr(bbJsonVal* pVal, int pos, const bbCHAR* str)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_STRING);
    v.u.string.ptr = (bbCHAR*)str;
    return bbJsonArrIns(pVal, pos, &v, 1);
}

bbJsonVal* bbJsonArrInsInt(bbJsonVal* pVal, int pos, bbS64 n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_INTEGER);
    v.u.integer = n;
    return bbJsonArrIns(pVal, pos, &v, 1);
}

bbJsonVal* bbJsonArrInsDbl(bbJsonVal* pVal, int pos, double n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_DOUBLE);
    v.u.dbl = n;
    return bbJsonArrIns(pVal, pos, &v, 1);
}

bbJsonVal* bbJsonArrInsBool(bbJsonVal* pVal, int pos, int n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_BOOLEAN);
    v.u.boolean = n;
    return bbJsonArrIns(pVal, pos, &v, 1);
}

void bbJsonArrDel(bbJsonVal* pVal, int pos, bbUINT count)
{
    bbUINT newSize, oldCapacity, newCapacity;
    bbJsonVal *pDel, *pDelBegin;

    if (pVal->mType != bbJSONTYPE_ARRAY || (bbUINT)pos >= pVal->u.array.length)
        return;

    if (pos + count > pVal->u.array.length)
        count = pVal->u.array.length - pos;

    newSize = pVal->u.array.length - count;

    pDelBegin = &pVal->u.array.values[pos];
    pDel = pDelBegin + count;
    while(pDel-- > pDelBegin)
        bbJsonValDestroy(pDel);
    bbMemMove(pDelBegin, pDelBegin + count, (newSize - pos) * sizeof(bbJsonVal));

    oldCapacity = bbJsonArrGetCapacity(pVal->u.array.length);
    newCapacity = bbJsonArrGetCapacity(newSize);
    if (oldCapacity != newCapacity)
        bbMemRealloc(newCapacity * sizeof(bbJsonVal), (void**)&pVal->u.array.values);

    pVal->u.array.length = newSize;
}

/*
 * Function bbJsonValInitParse is adapted from:
 *
 * Copyright (C) 2012 James McLaughlin et al.  All rights reserved.
 * https://github.com/udp/json-parser
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define e_off (i - cur_line_begin)

#define whitespace \
   case '\n': ++ cur_line;  cur_line_begin = i; \
   case ' ': case '\t': case '\r'

#define isdigit(b) (((bbUINT)b-'0')<=('9'-'0'))

static bbUINT hex_value(bbUINT cp)
{
    bbUINT val;
    if ((val = (bbUINT)cp - '0') > ('9'-'0'))
    {
        if ((val = ((bbUINT)cp&0xDF) - 'A') <= ('F'-'A'))
            val+=10;
        else
            return 0xFF;
    }
    return val;
}

#define flag_next           (1<<0)
#define flag_reproc         (1<<1)
#define flag_need_comma     (1<<2)
#define flag_seek_value     (1<<3)
#define flag_escaped        (1<<4)
#define flag_string         (1<<5)
#define flag_need_colon     (1<<6)
#define flag_done           (1<<7)
#define flag_num_negative   (1<<8)
#define flag_num_zero       (1<<9)
#define flag_num_e          (1<<10)
#define flag_num_e_got_sign (1<<11)
#define flag_num_e_negative (1<<12)

static bbJsonVal* bbJsonValInitParse_LinkParent(bbJsonVal* pParent, bbJsonVal* pVal)
{
    bbJsonVal* pValCopy;

    if (!pParent)
        return pVal;

    bbASSERT(pVal->reserved.mParent == NULL);

    if (pParent->mType == bbJSONTYPE_ARRAY)
    {
        pValCopy = bbJsonArrIns(pParent, -1, pVal, 1);
    }
    else if (pParent->mType == bbJSONTYPE_OBJECT)
    {
        bbMapPair* pair;
        pValCopy = bbJsonValCopy(pVal);
        pair = bbMapGetPair(&pParent->u.object, pParent->reserved.lastIdx);
        pair->val = (bbUPTR)pValCopy;
    }
    else
    {
        return NULL;
    }

    bbJsonValDestroy(pVal);
    pValCopy->reserved.mParent = pParent;
    return pValCopy;
}

bbERR bbJsonValInitParse(bbJsonVal* pRoot, const bbCHAR* json, bbUINT length)
{
    bbJsonVal* pVal = pRoot, *pParent = NULL;
    const bbCHAR* end = json + length;
    const bbCHAR *cur_line_begin, *i;
    unsigned int cur_line;
    long flags;
    long num_digits = 0, num_e = 0;
    bbS64 num_fraction = 0;
    bbUINT uc_b1, uc_b2, uc_b3, uc_b4;

    bbJsonVal val;
    bbStrBuf str;

    bbJsonValInit(pVal);
    bbStrBufInit(&str);
    bbgErrStr[0] = 0;

    flags = flag_seek_value;
    cur_line = 1;
    cur_line_begin = json;

    for (i=json ;; ++i)
    {
        bbCHAR b = (i == end ? 0 : *i);

        if (flags & flag_done)
        {
            if (!b)
                break;

            switch(b)
            {
            whitespace:
                continue;
            default:
                bbSprintf(bbgErrStr, bbT("%u:%zd: Trailing garbage: `%c`"), cur_line, e_off, b);
                goto e_failed;
            }
        }

        if (flags & flag_string)
        {
            if (!b)
            {
                bbSprintf(bbgErrStr, bbT("Unexpected EOF in string (at %u:%zd)"), cur_line, e_off);
                goto e_failed;
            }

            if (flags & flag_escaped)
            {
                flags &= ~ flag_escaped;

                switch (b)
                {
                case 'b': bbStrBufCatCP(&str, '\b'); break;
                case 'f': bbStrBufCatCP(&str, '\f'); break;
                case 'n': bbStrBufCatCP(&str, '\n'); break;
                case 'r': bbStrBufCatCP(&str, '\r'); break;
                case 't': bbStrBufCatCP(&str, '\t'); break;
                case 'u':
                    if ((uc_b1 = hex_value(*++i)) == 0xFF || (uc_b2 = hex_value(*++i)) == 0xFF ||
                        (uc_b3 = hex_value(*++i)) == 0xFF || (uc_b4 = hex_value(*++i)) == 0xFF)
                    {
                        bbSprintf(bbgErrStr, bbT("Invalid character value `%c` (at %u:%zd)"), b, cur_line, e_off);
                        goto e_failed;
                    }
                    uc_b1 = (uc_b1 << 4) + uc_b2;
                    uc_b2 = (uc_b3 << 4) + uc_b4;
                    bbStrBufCatCP(&str, ((bbCHARCP)uc_b1 << 8) + uc_b2);
                    break;
                default:
                    bbStrBufCatCP(&str, b);
                    break;
                }
                continue;
            }

            if (b == '\\')
            {
                flags |= flag_escaped;
                continue;
            }

            if (b == '"')
            {
                flags &= ~ flag_string;

                switch (pVal->mType)
                {
                case bbJSONTYPE_STRING:
                    pVal->u.string.length = bbStrBufGetLen(&str);
                    pVal->u.string.ptr = bbStrBufDetach(&str);
                    flags |= flag_next;
                    break;

                case bbJSONTYPE_OBJECT:
                    if ((pVal->reserved.lastIdx = bbMapAdd(&pVal->u.object, bbStrBufGetPtr(&str), 0)) < 0)
                        goto e_alloc_failure;
                    pParent = pVal;
                    pVal = &val;
                    flags |= flag_seek_value | flag_need_colon;
                    continue;

                default:
                    break;
                }
            }
            else
            {
                bbStrBufCatCP(&str, b);
                continue;
            }
        }

        if (flags & flag_seek_value)
        {
            switch (b)
            {
            whitespace:
                continue;

            case ']':
                if (!pParent || pParent->mType != bbJSONTYPE_ARRAY)
                {
                    bbSprintf(bbgErrStr, bbT("%u:%zd: Unexpected %c"), cur_line, e_off, b);
                    goto e_failed;
                }
                pVal = pParent;
                pParent = pVal->reserved.mParent;
                flags = (flags & ~ (flag_need_comma | flag_seek_value)) | flag_next;
                break;

            default:
                if (flags & flag_need_comma)
                {
                    if (b == ',')
                    {
                        flags &= ~ flag_need_comma;
                        continue;
                    }
                    else
                    {
                        bbSprintf(bbgErrStr, bbT("%u:%zd: Expected , before %c"), cur_line, e_off, b);
                        goto e_failed;
                    }
                }

                if (flags & flag_need_colon)
                {
                    if (b == ':')
                    {
                        flags &= ~ flag_need_colon;
                        continue;
                    }
                    else
                    {
                        bbSprintf(bbgErrStr, bbT("%u:%zd: Expected : before %c"), cur_line, e_off, b);
                        goto e_failed;
                    }
                }

                flags &= ~ flag_seek_value;

                switch (b)
                {
                case '{':
                    bbJsonValInitType(pVal, bbJSONTYPE_OBJECT);

                    if (!(pVal = bbJsonValInitParse_LinkParent(pParent, pVal)))
                        goto e_alloc_failure;

                    continue;

                case '[':
                    bbJsonValInitType(pVal, bbJSONTYPE_ARRAY);

                    if (!(pVal = bbJsonValInitParse_LinkParent(pParent, pVal)))
                        goto e_alloc_failure;

                    pParent = pVal;
                    pVal = &val;
                    flags |= flag_seek_value;
                    continue;

                case '"':
                    bbJsonValInitType(pVal, bbJSONTYPE_STRING);

                    if (!(pVal = bbJsonValInitParse_LinkParent(pParent, pVal)))
                        goto e_alloc_failure;

                    bbStrBufClear(&str);

                    flags |= flag_string;
                    continue;

                 case 't':
                    if ((end - i) < 3 || *(++ i) != 'r' || *(++ i) != 'u' || *(++ i) != 'e')
                        goto e_unknown_value;

                    bbJsonValInitType(pVal, bbJSONTYPE_BOOLEAN);
                    pVal->u.boolean = 1;

                    if (!(bbJsonValInitParse_LinkParent(pParent, pVal)))
                        goto e_alloc_failure;

                    flags |= flag_next;
                    break;

                 case 'f':
                    if ((end - i) < 4 || *(++ i) != 'a' || *(++ i) != 'l' || *(++ i) != 's' || *(++ i) != 'e')
                        goto e_unknown_value;

                    bbJsonValInitType(pVal, bbJSONTYPE_BOOLEAN);

                    if (!(bbJsonValInitParse_LinkParent(pParent, pVal)))
                        goto e_alloc_failure;

                    flags |= flag_next;
                    break;

                 case 'n':
                    if ((end - i) < 3 || *(++ i) != 'u' || *(++ i) != 'l' || *(++ i) != 'l')
                        goto e_unknown_value;

                    bbJsonValInitType(pVal, bbJSONTYPE_NULL);

                    if (!(bbJsonValInitParse_LinkParent(pParent, pVal)))
                        goto e_alloc_failure;

                    flags |= flag_next;
                    break;

                 default:
                    if (isdigit(b) || b == '-')
                    {
                        bbJsonValInitType(pVal, bbJSONTYPE_INTEGER);

                        if (!(pVal = bbJsonValInitParse_LinkParent(pParent, pVal)))
                            goto e_alloc_failure;

                        flags &= ~ (flag_num_negative | flag_num_e | flag_num_e_got_sign | flag_num_e_negative | flag_num_zero);

                        num_digits = 0;
                        num_fraction = 0;
                        num_e = 0;

                        if (b != '-')
                        {
                            flags |= flag_reproc;
                            break;
                        }

                        flags |= flag_num_negative;
                        continue;
                    }
                    else
                    {
                        bbSprintf(bbgErrStr, bbT("%u:%zd: Unexpected %c when seeking value"), cur_line, e_off, b);
                        goto e_failed;
                    }
                }
            }
        }
        else
        {
            switch (pVal->mType)
            {
            case bbJSONTYPE_OBJECT:
                switch (b)
                {
                whitespace:
                    continue;

                case '"':
                    flags |= flag_string;
                    bbStrBufClear(&str);
                    break;

                case '}':
                    flags = (flags & ~ flag_need_comma) | flag_next;
                    break;

                case ',':
                    if (flags & flag_need_comma)
                    {
                        flags &= ~ flag_need_comma;
                        break;
                    }

                default:
                    bbSprintf(bbgErrStr, bbT("%u:%zd: Unexpected `%c` in object"), cur_line, e_off, b);
                    goto e_failed;
                }
                break;

            case bbJSONTYPE_INTEGER:
            case bbJSONTYPE_DOUBLE:
                if (isdigit(b))
                {
                    ++num_digits;

                    if (pVal->mType == bbJSONTYPE_INTEGER || flags & flag_num_e)
                    {
                        if (! (flags & flag_num_e))
                        {
                            if (flags & flag_num_zero)
                            {
                                bbSprintf(bbgErrStr, bbT("%u:%zd: Unexpected `0` before `%c`"), cur_line, e_off, b);
                                goto e_failed;
                            }

                            if (num_digits == 1 && b == '0')
                                flags |= flag_num_zero;
                        }
                        else
                        {
                            flags |= flag_num_e_got_sign;
                            num_e = (num_e * 10) + (b - '0');
                            continue;
                        }

                        pVal->u.integer = (pVal->u.integer * 10) + (b - '0');
                        continue;
                    }

                    num_fraction = (num_fraction * 10) + (b - '0');
                    continue;
                }

                if (b == '+' || b == '-')
                {
                    if ( (flags & flag_num_e) && !(flags & flag_num_e_got_sign))
                    {
                        flags |= flag_num_e_got_sign;
                        if (b == '-')
                            flags |= flag_num_e_negative;
                        continue;
                    }
                }
                else if (b == '.' && pVal->mType == bbJSONTYPE_INTEGER)
                {
                    if (!num_digits)
                    {
                        bbSprintf(bbgErrStr, bbT("%u:%zd: Expected digit before `.`"), cur_line, e_off);
                        goto e_failed;
                    }

                    pVal->mType = bbJSONTYPE_DOUBLE;
                    pVal->u.dbl = (double) pVal->u.integer;

                    num_digits = 0;
                    continue;
                }

                if (! (flags & flag_num_e))
                {
                    if (pVal->mType == bbJSONTYPE_DOUBLE)
                    {
                        if (!num_digits)
                        {
                            bbSprintf(bbgErrStr, bbT("%u:%zd: Expected digit after `.`"), cur_line, e_off);
                            goto e_failed;
                        }

                        pVal->u.dbl += ((double) num_fraction) / (pow(10, (double) num_digits));
                    }

                    if (b == 'e' || b == 'E')
                    {
                        flags |= flag_num_e;

                        if (pVal->mType == bbJSONTYPE_INTEGER)
                        {
                            pVal->mType = bbJSONTYPE_DOUBLE;
                            pVal->u.dbl = (double) pVal->u.integer;
                        }

                        num_digits = 0;
                        flags &= ~ flag_num_zero;
                        continue;
                    }
                }
                else
                {
                    if (!num_digits)
                    {
                        bbSprintf(bbgErrStr, bbT("%u:%zd: Expected digit after `e`"), cur_line, e_off);
                        goto e_failed;
                    }

                    pVal->u.dbl *= pow (10, (double) (flags & flag_num_e_negative ? - num_e : num_e));
                }

                if (flags & flag_num_negative)
                {
                    if (pVal->mType == bbJSONTYPE_INTEGER)
                        pVal->u.integer = - pVal->u.integer;
                    else
                        pVal->u.dbl = - pVal->u.dbl;
                }

                flags |= flag_next | flag_reproc;
                break;

            default:
                break;
            }
        }

        if (flags & flag_reproc)
        {
            flags &= ~ flag_reproc;
            --i;
        }

        if (flags & flag_next)
        {
            flags = (flags & ~ flag_next) | flag_need_comma;

            if (!pParent)
            {
                flags |= flag_done; /* root value done */
                continue;
            }

            if (pParent->mType == bbJSONTYPE_ARRAY)
            {
                flags |= flag_seek_value;
                pVal = &val;
            }
            else
            {
                pVal = pParent;
                pParent = pVal->reserved.mParent;
            }

            continue;
        }
    }

    bbStrBufDestroy(&str);
    return bbEOK;

e_unknown_value:
    bbSprintf(bbgErrStr, bbT("%u:%zd: Unknown value"), cur_line, e_off);
    goto e_failed;

e_alloc_failure:
    bbStrCpy(bbgErrStr, bbT("Memory allocation failure"));
    goto e_failed;

e_failed:
    if (!bbgErrStr[0])
        bbStrCpy(bbgErrStr, bbT("Unknown error"));

    bbJsonValDestroy(pRoot);
    bbStrBufDestroy(&str);
    return bbErrSet(bbEJSONSYNTAX);
}

