/*
 * Adapted for https://github.com/mrhill/babel
 *
 * Original copyright notice:
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

#include "json.h"
#include "fixmath.h"

void bbJsonValInit(bbJsonVal* pVal)
{
    bbMemClear(pVal, sizeof(bbJsonVal));
}

void bbJsonValInitType(bbJsonVal* pVal, bbJSONTYPE type)
{
    pVal->mParent = NULL;
    pVal->mType = type;

    if (type == bbJSONTYPE_OBJECT)
        bbMapInit(&pVal->u.object);
    else
        bbMemClear(&pVal->u, sizeof(pVal->u));
}

void bbJsonValDestroy(bbJsonVal* value)
{
    bbJsonVal* cur_value;
    bbUINT last;

    if (!value)
       return;

    value->mParent = 0;

    while (value)
    {
        switch (value->mType)
        {
        case bbJSONTYPE_ARRAY:
            if (!value->u.array.length)
            {
                bbMemFree(value->u.array.values);
                break;
            }

            value = value->u.array.values [-- value->u.array.length];
            continue;

        case bbJSONTYPE_OBJECT:
            if (!bbMapGetSize(&value->u.object))
            {
                bbMapDestroy(&value->u.object);
                break;
            }

            last = bbMapGetSize(&value->u.object) - 1;
            cur_value = (bbJsonVal*)(bbUPTR)(bbMapGetPair(&value->u.object, last)->val);
            bbMapDelIndex(&value->u.object, last);
            value = cur_value;
            continue;

        case bbJSONTYPE_STRING:
             bbMemFree(value->u.string.ptr);
             break;

        default:
             break;
        };

        cur_value = value;
        value = value->mParent;
        if (value)
            bbMemFree(cur_value);
    }
}

bbERR bbJsonValDump(const bbJsonVal* v, bbStrBuf* s)
{
    bbUINT i;

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
            bbStrBufCatf(s, bbT("\"%s\":"), bbMapGetPair(&v->u.object, i)->key);
            if (bbEOK != bbJsonValDump((bbJsonVal*)(bbUPTR)bbMapGetPair(&v->u.object, i)->val, s))
                return bbELAST;
        }
        bbStrBufCatCP(s, '}');
        break;

    case bbJSONTYPE_ARRAY:
        bbStrBufCatCP(s, '[');
        for (i=0; i<v->u.array.length; ++i)
        {
            if (i) bbStrBufCatCP(s, ',');
            if (bbEOK != bbJsonValDump(v->u.array.values[i], s))
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

bbERR bbJsonValInitCopy(bbJsonVal* pNew, const bbJsonVal* pVal)
{
    pNew->mParent = pVal->mParent;
    pNew->mType = pVal->mType;
    switch(pVal->mType)
    {
    case bbJSONTYPE_OBJECT:
        if (bbEOK != bbMapInitCopy(&pNew->u.object, &pVal->u.object))
            return bbELAST;
        break;

    case bbJSONTYPE_ARRAY:
        pNew->u.array.values = bbMemAlloc(pVal->u.array.length * sizeof(struct bbJsonVal*));
        if (!pNew->u.array.values)
            return bbELAST;
        pNew->u.array.length = pVal->u.array.length;
        bbMemCpy(pNew->u.array.values, pVal->u.array.values, pVal->u.array.length  * sizeof(struct bbJsonVal*));
        break;

    case bbJSONTYPE_STRING:
        pNew->u.string.ptr = bbStrDup(pVal->u.string.ptr);
        if (!pNew->u.string.ptr)
            return bbELAST;
        pNew->u.string.length = pVal->u.string.length;
        break;

    default:
        pNew->u.integer = pVal->u.integer;
        break;
    }
    return bbEOK;
}

bbJsonVal* bbJsonValCopy(const bbJsonVal* pVal)
{
    bbJsonVal* pNew = bbMemAlloc(sizeof(bbJsonVal));
    if (pNew)
    {
        if (bbEOK != bbJsonValInitCopy(pNew, pVal))
            bbMemFreeNull((void**)&pNew);
    }
    return pNew;
}

bbERR bbJsonObjAddObj(bbJsonVal* pVal, const bbCHAR* key, const bbJsonVal* pObj)
{
    bbJsonVal* pNew;

    if (pVal->mType == bbJSONTYPE_NONE)
        pVal->mType = bbJSONTYPE_OBJECT;

    if (pVal->mType != bbJSONTYPE_OBJECT)
        return bbErrSet(bbEBADPARAM);

    if (!(pNew = bbJsonValCopy(pObj)))
        return bbELAST;

    if (bbEOK != bbMapAdd(&pVal->u.object, key, (bbUPTR)pNew))
    {
        bbJsonValDestroy(pNew);
        return bbELAST;
    }

    pNew->mParent = pVal;

    return bbEOK;
}

bbERR bbJsonObjAddStr(bbJsonVal* pVal, const bbCHAR* key, const bbCHAR* str)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_STRING);
    v.u.string.ptr = (bbCHAR*)str;
    return bbJsonObjAddObj(pVal, key, &v);
}

bbERR bbJsonObjAddInt(bbJsonVal* pVal, const bbCHAR* key, bbS64 n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_INTEGER);
    v.u.integer = n;
    return bbJsonObjAddObj(pVal, key, &v);
}

bbERR bbJsonObjAddDbl(bbJsonVal* pVal, const bbCHAR* key, double n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_DOUBLE);
    v.u.dbl = n;
    return bbJsonObjAddObj(pVal, key, &v);
}

bbERR bbJsonObjAddBool(bbJsonVal* pVal, const bbCHAR* key, int n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_BOOLEAN);
    v.u.boolean = n;
    return bbJsonObjAddObj(pVal, key, &v);
}

bbERR bbJsonArrInsObj(bbJsonVal* pVal, int pos, const bbJsonVal* pObj)
{
    bbJsonVal* pNew;
    bbJsonVal** pInsert;
    bbUINT capacity = 0;

    if (pVal->mType == bbJSONTYPE_NONE)
        pVal->mType = bbJSONTYPE_ARRAY;

    if (pVal->mType != bbJSONTYPE_ARRAY)
        return bbErrSet(bbEBADPARAM);

    if ((bbUINT)pos > pVal->u.array.length)
        pos = pVal->u.array.length;

    if (!(pNew = bbJsonValCopy(pObj)))
        return bbELAST;

    if (pVal->u.array.length)
        capacity = 2 << bbGetTopBit(pVal->u.array.length - 1);

    if ((pVal->u.array.length + 1) > capacity)
    {
        capacity = capacity ? capacity<<1 : 2;
        if (bbEOK != bbMemRealloc(capacity * sizeof(struct bbJsonVal*), (void**)&pVal->u.array.values))
        {
            bbJsonValDestroy(pNew);
            return bbELAST;
        }
    }

    pInsert = pVal->u.array.values + pos;
    bbMemMove(pInsert + 1, pInsert, (pVal->u.array.length - pos) * sizeof(bbJsonVal*));
    *pInsert = pNew;
    pVal->u.array.length++;
    pNew->mParent = pVal;

    return bbEOK;
}

bbERR bbJsonArrInsStr(bbJsonVal* pVal, int pos, const bbCHAR* str)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_STRING);
    v.u.string.ptr = (bbCHAR*)str;
    return bbJsonArrInsObj(pVal, pos, &v);
}

bbERR bbJsonArrInsInt(bbJsonVal* pVal, int pos, bbS64 n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_INTEGER);
    v.u.integer = n;
    return bbJsonArrInsObj(pVal, pos, &v);
}

bbERR bbJsonArrInsDbl(bbJsonVal* pVal, int pos, double n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_DOUBLE);
    v.u.dbl = n;
    return bbJsonArrInsObj(pVal, pos, &v);
}

bbERR bbJsonArrInsBool(bbJsonVal* pVal, int pos, int n)
{
    bbJsonVal v;
    bbJsonValInitType(&v, bbJSONTYPE_BOOLEAN);
    v.u.boolean = n;
    return bbJsonArrInsObj(pVal, pos, &v);
}


