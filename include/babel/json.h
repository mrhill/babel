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

/** Construct a Json node of type bbJSONTYPE_NONE.
    Always succeeds.
    @param pVal Unitialized Json node
*/
void    bbJsonValInit(bbJsonVal* pVal);

/** Construct a Json node of given type.
    Always succeeds, nodes of type object, array, or string will be created with size 0.
    @param pVal Unitialized Json node
*/
void    bbJsonValInitType(bbJsonVal* pVal, bbJSONTYPE type);

/** Construct a Json node with deepcopy of given node.
    @param pVal Unitialized Json node
    @param pOther Valid node to copy, can be NULL to create bbJSONTYPE_NONE
*/
bbERR   bbJsonValInitCopy(bbJsonVal* pVal, const bbJsonVal* pOther);

/** Construct a Json node from serialized JSON text.
    @param pVal Unitialized Json node
    @param text JSON-formatted text
    @param length Size of text in number of characters, or (bbUINT)-1 to stop at 0-terminator
*/
bbERR   bbJsonValInitParse(bbJsonVal* pVal, const bbCHAR* text, bbUINT length);

/** Destroy Json node and all its subnodes.
    On exit the Json node will still be valid and of type bbJSONTYPE_NONE.
    Destroying a node twice is safe.
    @param pVal Json node to destroy
*/
void    bbJsonValDestroy(bbJsonVal* pVal);

/** Clear json value and all its subnodes.
    Sets the node to bbJSONTYPE_NONE, but does not unlink it from its parent.
    @param pVal Json node
*/
void    bbJsonValClear(bbJsonVal* pVal);

/** Assign Json node contents.
    Replaces the Json node \a pVal with a deepcopy of \a pOther, but does
    not unlink \a pVal from its parent.
    @param pVal Json node to replace, must be initialized
    @param pOther Json contents to assign, can be NULL to create bbJSONTYPE_NONE
*/
bbERR   bbJsonValAssign(bbJsonVal* pVal, const bbJsonVal* pOther);

/** Dump Json tree into JSON-formatted text.
    @param pVal Node to start dumping at
    @param pStr String buffer
    @param indent 0 for unformatted, 1 to format with indention
*/
bbERR   bbJsonValDump(const bbJsonVal* pVal, bbStrBuf* pStr, bbUINT indent);

bbERR   bbJsonObjAddObj(bbJsonVal* pVal, const bbCHAR* key, const bbJsonVal* pObj);
bbERR   bbJsonObjAddStr(bbJsonVal* pVal, const bbCHAR* key, const bbCHAR* str);
bbERR   bbJsonObjAddInt(bbJsonVal* pVal, const bbCHAR* key, bbS64 n);
bbERR   bbJsonObjAddDbl(bbJsonVal* pVal, const bbCHAR* key, double n);
bbERR   bbJsonObjAddBool(bbJsonVal* pVal, const bbCHAR* key, int n);
void    bbJsonObjDel(bbJsonVal* pVal, const bbCHAR* key);
#define bbJsonObjGet(pVal, key) ((bbJsonVal*)(bbUPTR)bbMapGet(&(pVal)->u.object, key))

bbJsonVal* bbJsonArrInsObj(bbJsonVal* pVal, int pos, const bbJsonVal* pObj);
bbJsonVal* bbJsonArrInsStr(bbJsonVal* pVal, int pos, const bbCHAR* str);
bbJsonVal* bbJsonArrInsInt(bbJsonVal* pVal, int pos, bbS64 n);
bbJsonVal* bbJsonArrInsDbl(bbJsonVal* pVal, int pos, double n);
bbJsonVal* bbJsonArrInsBool(bbJsonVal* pVal, int pos, int n);
void       bbJsonArrDel(bbJsonVal* pVal, int pos);
#define    bbJsonArrGetSize(pVal) ((pVal)->u.array.length)


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

    union {
        int lastIdx;
    } reserved;

    #ifdef  __cplusplus
    bbJsonVal() { bbJsonValInit(this); }
    bbJsonVal(bbJSONTYPE type) { bbJsonValInitType(this, type); }
    bbJsonVal(const bbJsonVal& other) { bbJsonValInitCopy(this, &other); }
    ~bbJsonVal() { bbJsonValDestroy(this); }
    void Clear() { bbJsonValClear(this); }
    bbJsonVal& operator=(const bbJsonVal& other) { bbJsonValAssign(this, &other); return *this; }
    bbERR Dump(bbStrBuf& str, bbUINT indent) const { return bbJsonValDump(this, &str, indent); }

    const bbJsonVal& operator[](const bbCHAR* key) const { return *(const bbJsonVal*)bbMapGet(&u.object, key); }
    const bbJsonVal& operator[](bbUINT idx) const { return u.array.values[idx]; }
    #endif
};

#ifdef  __cplusplus
}
#endif

#endif

