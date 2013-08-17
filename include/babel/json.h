#ifndef _bbVal_H_
#define _bbVal_H_

#include "map.h"
#include "strbuf.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** JSON node types */
typedef enum
{
   bbJSONTYPE_NONE = 0,
   bbJSONTYPE_OBJECT,
   bbJSONTYPE_ARRAY,
   bbJSONTYPE_INTEGER,
   bbJSONTYPE_DOUBLE,
   bbJSONTYPE_STRING,
   bbJSONTYPE_BOOLEAN,
   bbJSONTYPE_NULL

} bbJSONTYPE;

struct bbJsonVal;

#ifndef  __cplusplus
typedef struct bbJsonVal bbJsonVal;
#endif

void  bbJsonValInit(bbJsonVal* pVal);
void  bbJsonValInitType(bbJsonVal* pVal, bbJSONTYPE type);
bbERR bbJsonValInitCopy(bbJsonVal* pVal, const bbJsonVal* pOther);
void  bbJsonValDestroy(bbJsonVal* pVal);

bbJsonVal* bbJsonValCopy(const bbJsonVal* pVal);

bbERR bbJsonObjAddObj(bbJsonVal* pVal, const bbCHAR* key, const bbJsonVal* pObj);
bbERR bbJsonObjAddStr(bbJsonVal* pVal, const bbCHAR* key, const bbCHAR* str);
bbERR bbJsonObjAddInt(bbJsonVal* pVal, const bbCHAR* key, bbS64 n);
bbERR bbJsonObjAddDbl(bbJsonVal* pVal, const bbCHAR* key, double n);
bbERR bbJsonObjAddBool(bbJsonVal* pVal, const bbCHAR* key, int n);
void  bbJsonObjDel(bbJsonVal* pVal, const bbCHAR* key);
#define bbJsonObjGet(pVal, key) ((bbJsonVal*)(bbUPTR)bbMapGet(&(pVal)->u.object, key))

bbERR  bbJsonArrInsObj(bbJsonVal* pVal, int pos, const bbJsonVal* pObj);
bbERR  bbJsonArrInsStr(bbJsonVal* pVal, int pos, const bbCHAR* str);
bbERR  bbJsonArrInsInt(bbJsonVal* pVal, int pos, bbS64 n);
bbERR  bbJsonArrInsDbl(bbJsonVal* pVal, int pos, double n);
bbERR  bbJsonArrInsBool(bbJsonVal* pVal, int pos, int n);
void   bbJsonArrDel(bbJsonVal* pVal, int pos);
#define bbJsonArrGetSize(pVal) ((pVal)->u.array.length)

bbERR bbJsonValDump(const bbJsonVal* v, bbStrBuf* s, bbUINT indent);

bbJsonVal* bbJsonParse(const char* text, bbUINT length);

struct bbJsonVal
{
    struct bbJsonVal* mParent;
    bbJSONTYPE        mType;

    union {
        int    boolean;
        bbS64  integer;
        double dbl;

        struct
        {
            bbCHAR* ptr;
            bbUINT  length;

        } string;

        struct
        {
            struct bbJsonVal* values;
            bbUINT length;
        } array;

        struct bbMapRec object;
    } u;

    #ifdef  __cplusplus
    bbJsonVal() { bbJsonValInit(this); }
    bbJsonVal(bbJSONTYPE type) { bbJsonValInitType(this, type); }
    ~bbJsonVal() { bbJsonValDestroy(this); }
    bbERR Dump(bbStrBuf& s) const { return bbJsonValDump(this, &s); }

    const bbJsonVal& operator[](const bbCHAR* key) const { return *(const bbJsonVal*)bbMapGet(&u.object, key); }
    const bbJsonVal& operator[](bbUINT idx) const { return &u.array.values[idx]; }
    #endif
};

#ifdef  __cplusplus
}
#endif

#endif

