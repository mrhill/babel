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
    Sets the node to bbJSONTYPE_NONE.
    @param pVal Json node
*/
#define bbJsonValClear(pVal) bbJsonValDestroy(pVal)

/** Assign Json node contents.
    Replaces the Json node \a pVal with a deepcopy of \a pOther.
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

/** Dump Json tree into JSON-formatted textfile.
    @param pVal Node to start dumping at
    @param pFile Filename to save to
    @param indent 0 for unformatted, 1 to format with indention
*/
bbERR   bbJsonValSave(const bbJsonVal* pVal, const bbCHAR* pFile, bbUINT indent);

/** Load serialized JSON text to node.
    @param pVal Itialized Json node
    @param pFile Filename to load from
*/
bbERR   bbJsonValLoad(bbJsonVal* v, const bbCHAR* pFile);

/** To node of type object append a deepcopy of given Json node.
    @param pVal Json node to add to, must be of bbJSONTYPE_OBJECT, or bbJSONTYPE_NONE
    @param key  Key to create
    @param pObj Json contents to add, can be NULL to create bbJSONTYPE_NONE
*/
bbJsonVal* bbJsonObjAdd(bbJsonVal* pVal, const bbCHAR* key, const bbJsonVal* pObj);
bbJsonVal* bbJsonObjAddStr(bbJsonVal* pVal, const bbCHAR* key, const bbCHAR* str);
bbJsonVal* bbJsonObjAddInt(bbJsonVal* pVal, const bbCHAR* key, bbS64 n);
bbJsonVal* bbJsonObjAddDbl(bbJsonVal* pVal, const bbCHAR* key, double n);
bbJsonVal* bbJsonObjAddBool(bbJsonVal* pVal, const bbCHAR* key, int n);
void       bbJsonObjDel(bbJsonVal* pVal, const bbCHAR* key);
#define    bbJsonObjGet(pVal, key) ((bbJsonVal*)(bbUPTR)bbMapGet(&(pVal)->u.object, key))
bbS64      bbJsonObjGetInt(const bbJsonVal* pVal, const bbCHAR* key, bbS64 dflt);
int        bbJsonObjGetBool(const bbJsonVal* pVal, const bbCHAR* key, int dflt);
#define    bbJsonObjGetSize(pVal) bbMapGetSize(&(pVal)->u.object)
#define    bbJsonObjGetPair(pVal, index) bbMapGetPair(&(pVal)->u.object, (index))

/** To node of type array append a deepcopies of given Json nodes.
    @param pVal Json node to add to, must be of bbJSONTYPE_ARRAY, or bbJSONTYPE_NONE
    @param pos  Insert position in array, can be invalid to append at end
    @param pObj Array of Json contents to add, can be NULL to create bbJSONTYPE_NONE
    @param count Number of elements to insert
    @return Pointer to first node inserted, or NULL on failure
*/
bbJsonVal* bbJsonArrIns(bbJsonVal* pVal, int pos, const bbJsonVal* pObj, bbUINT count);
bbJsonVal* bbJsonArrInsStr(bbJsonVal* pVal, int pos, const bbCHAR* str);
bbJsonVal* bbJsonArrInsInt(bbJsonVal* pVal, int pos, bbS64 n);
bbJsonVal* bbJsonArrInsDbl(bbJsonVal* pVal, int pos, double n);
bbJsonVal* bbJsonArrInsBool(bbJsonVal* pVal, int pos, int n);

/** Delete a segment of Json array.
    @param pVal Json node to delete from, must be of bbJSONTYPE_ARRAY
    @param pos Start index to delete from, do nothing if <0 or larger than array size
    @param count Number of nodes to delete, count is truncated at array size
*/
void       bbJsonArrDel(bbJsonVal* pVal, int pos, bbUINT count);
#define    bbJsonArrGetSize(pVal) ((pVal)->u.array.length)
#define    bbJsonArrGet(pVal, idx) ((pVal)->u.array.values + (idx))

/** To node of type array append a copy of integer array by creating as many Json nodes.
    @param pVal Json node to add to, must be of bbJSONTYPE_ARRAY, or bbJSONTYPE_NONE
    @param pos  Insert position in array, can be invalid to append at end
    @param pArr Array to add
    @param count Number of elements in array
    @return Pointer to first node inserted, or NULL on failure
*/
bbJsonVal* bbJsonArrInsArrU8(bbJsonVal* pVal, int pos, const bbU8* pArr, bbUINT count);
bbJsonVal* bbJsonArrInsArrU16(bbJsonVal* pVal, int pos, const bbU16* pArr, bbUINT count);
bbJsonVal* bbJsonArrInsArrU32(bbJsonVal* pVal, int pos, const bbU32* pArr, bbUINT count);
bbJsonVal* bbJsonArrInsArrU64(bbJsonVal* pVal, int pos, const bbU64* pArr, bbUINT count);


struct bbJsonVal
{
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

    struct {
        struct bbJsonVal* mParent;
        int lastIdx;
    } reserved;

    #ifdef  __cplusplus
    bbJsonVal() { bbJsonValInit(this); }
    bbJsonVal(bbJSONTYPE type) { bbJsonValInitType(this, type); }
    bbJsonVal(const bbJsonVal& other) { bbJsonValInitCopy(this, &other); }

    #ifdef _MSC_VER
    bbJsonVal(int v)    { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    bbJsonVal(bbUINT v) { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    #endif
    bbJsonVal(bbS64  v) { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    bbJsonVal(bbU64  v) { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    bbJsonVal(bbS32  v) { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    bbJsonVal(bbU32  v) { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    bbJsonVal(bbS16  v) { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    bbJsonVal(bbU16  v) { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    bbJsonVal(bbS8   v) { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    bbJsonVal(bbU8   v) { bbJsonValInitType(this, bbJSONTYPE_INTEGER); this->u.integer=v; }
    bbJsonVal(bool   v) { bbJsonValInitType(this, bbJSONTYPE_BOOLEAN); this->u.boolean=v; }
    bbJsonVal(const bbCHAR* v) { bbJsonValInitType(this, bbJSONTYPE_STRING);  if (v) this->u.string.ptr = bbStrDup(v), this->u.string.length = bbStrLen(v); }

    bbJsonVal(const bbU8*  arr, bbUINT count) { bbJsonValInit(this); bbJsonArrInsArrU8 (this, 0, arr, count); }
    bbJsonVal(const bbU16* arr, bbUINT count) { bbJsonValInit(this); bbJsonArrInsArrU16(this, 0, arr, count); }
    bbJsonVal(const bbU32* arr, bbUINT count) { bbJsonValInit(this); bbJsonArrInsArrU32(this, 0, arr, count); }
    bbJsonVal(const bbU64* arr, bbUINT count) { bbJsonValInit(this); bbJsonArrInsArrU64(this, 0, arr, count); }

    ~bbJsonVal() { bbJsonValDestroy(this); }
    void Clear() { bbJsonValClear(this); }
    inline bool IsValid() const { return this && this->mType != bbJSONTYPE_NONE; }
    inline bbJSONTYPE GetType() const { return this ? this->mType : bbJSONTYPE_NONE; }

    inline bbJsonVal& operator=(const bbJsonVal& other) { bbJsonValAssign(this, &other); return *this; }

    struct ArrU8  { const bbU8*  ptr; bbUINT size; ArrU8 (const bbU8*  p, bbUINT s):ptr(p),size(s){} };
    struct ArrU16 { const bbU16* ptr; bbUINT size; ArrU16(const bbU16* p, bbUINT s):ptr(p),size(s){} };
    struct ArrU32 { const bbU32* ptr; bbUINT size; ArrU32(const bbU32* p, bbUINT s):ptr(p),size(s){} };
    struct ArrU64 { const bbU64* ptr; bbUINT size; ArrU64(const bbU64* p, bbUINT s):ptr(p),size(s){} };
    inline bbJsonVal& operator=(const ArrU8 & arr) { bbJsonValAssign(this, NULL); bbJsonArrInsArrU8 (this, 0, arr.ptr, arr.size); return *this; }
    inline bbJsonVal& operator=(const ArrU16& arr) { bbJsonValAssign(this, NULL); bbJsonArrInsArrU16(this, 0, arr.ptr, arr.size); return *this; }
    inline bbJsonVal& operator=(const ArrU32& arr) { bbJsonValAssign(this, NULL); bbJsonArrInsArrU32(this, 0, arr.ptr, arr.size); return *this; }
    inline bbJsonVal& operator=(const ArrU64& arr) { bbJsonValAssign(this, NULL); bbJsonArrInsArrU64(this, 0, arr.ptr, arr.size); return *this; }

    inline bbU32 u8() const { return this ? (bbU8)u.integer : 0; }   /**< Access JSON integer node as bbU8. */
    inline bbU32 u16() const { return this ? (bbU16)u.integer : 0; } /**< Access JSON integer node as bbU16. */
    inline bbU32 u32() const { return this ? (bbU32)u.integer : 0; } /**< Access JSON integer node as bbU32. */
    inline bbU32 u64() const { return this ? (bbU64)u.integer : 0; } /**< Access JSON integer node as bbU64. */

    inline bbERR Dump(bbStrBuf& str, bbUINT indent) const { return bbJsonValDump(this, &str, indent); }
    inline bbERR Save(const bbCHAR* pFile, bbUINT indent) const { return bbJsonValSave(this, pFile, indent); }
    inline bbERR Load(const bbCHAR* pFile) { return bbJsonValLoad(this, pFile); }
    inline void Print() const { bbStrBuf s; if (bbEOK == Dump(s, 1)) bbPrintf("%s\n", s.GetPtr()); }

    inline bbJsonVal* ObjAdd(const bbCHAR* key, const bbJsonVal* pObj) { return bbJsonObjAdd(this, key, pObj); }
    inline const bbJsonVal* ObjGet(const bbCHAR* key) const { return bbJsonObjGet(this, key); }
    inline bbUINT ObjGetSize() const { return bbJsonObjGetSize(this); }
    inline const bbMapPair* ObjGetPair(bbUINT index) const { return bbJsonObjGetPair(this, index); }

    inline bbU8 GetU8(const bbCHAR* key, bbU8 dflt=0) const { return (bbU8)bbJsonObjGetInt(this, key, dflt); }
    inline bbS8 GetS8(const bbCHAR* key, bbU8 dflt=0) const { return (bbS8)bbJsonObjGetInt(this, key, dflt); }
    inline bbU16 GetU16(const bbCHAR* key, bbU16 dflt=0) const { return (bbU16)bbJsonObjGetInt(this, key, dflt); }
    inline bbS16 GetS16(const bbCHAR* key, bbU16 dflt=0) const { return (bbS16)bbJsonObjGetInt(this, key, dflt); }
    inline bbU32 GetU32(const bbCHAR* key, bbU32 dflt=0) const { return (bbU32)bbJsonObjGetInt(this, key, dflt); }
    inline bbS32 GetS32(const bbCHAR* key, bbU32 dflt=0) const { return (bbS32)bbJsonObjGetInt(this, key, dflt); }
    inline bbU64 GetU64(const bbCHAR* key, bbU64 dflt=0) const { return (bbU64)bbJsonObjGetInt(this, key, dflt); }
    inline bbS64 GetS64(const bbCHAR* key, bbU64 dflt=0) const { return (bbS64)bbJsonObjGetInt(this, key, dflt); }
    inline bool GetBool(const bbCHAR* key, bool dflt=false) const { return bbJsonObjGetBool(this, key, dflt); }

    inline const bbJsonVal* SetU8(const bbCHAR* key, bbU8 val) { return bbJsonObjAddInt(this, key, val); }
    inline const bbJsonVal* SetS8(const bbCHAR* key, bbS8 val) { return bbJsonObjAddInt(this, key, val); }
    inline const bbJsonVal* SetU16(const bbCHAR* key, bbU16 val) { return bbJsonObjAddInt(this, key, val); }
    inline const bbJsonVal* SetS16(const bbCHAR* key, bbS16 val) { return bbJsonObjAddInt(this, key, val); }
    inline const bbJsonVal* SetU32(const bbCHAR* key, bbU32 val) { return bbJsonObjAddInt(this, key, val); }
    inline const bbJsonVal* SetS32(const bbCHAR* key, bbS32 val) { return bbJsonObjAddInt(this, key, val); }
    inline const bbJsonVal* SetU64(const bbCHAR* key, bbU64 val) { return bbJsonObjAddInt(this, key, val); }
    inline const bbJsonVal* SetS64(const bbCHAR* key, bbS64 val) { return bbJsonObjAddInt(this, key, val); }
    inline const bbJsonVal* SetBool(const bbCHAR* key, bool val) { return bbJsonObjAddBool(this, key, val); }
    inline const bbJsonVal* SetF32(const bbCHAR* key, float val) { return bbJsonObjAddDbl(this, key, val); }
    inline const bbJsonVal* SetF64(const bbCHAR* key, double val) { return bbJsonObjAddDbl(this, key, val); }

    inline bbUINT ArrGetSize() const { return bbJsonArrGetSize(this); }
    inline const bbJsonVal* ArrGet(bbUINT idx) const { return bbJsonArrGet(this, idx); }
    inline bbJsonVal* ArrIns(int pos, const bbJsonVal* pObj, bbUINT count=1) { return bbJsonArrIns(this, pos, pObj, count); }
    inline void ArrDel(int pos, bbUINT count = 1) { bbJsonArrDel(this, pos, count); }

    inline bbJsonVal* InsArrU8(int pos, const bbU8* pArr, bbUINT count) { return bbJsonArrInsArrU8(this, pos, pArr, count); }
    inline bbJsonVal* InsArrU16(int pos, const bbU16* pArr, bbUINT count) { return bbJsonArrInsArrU16(this, pos, pArr, count); }
    inline bbJsonVal* InsArrU32(int pos, const bbU32* pArr, bbUINT count) { return bbJsonArrInsArrU32(this, pos, pArr, count); }
    inline bbJsonVal* InsArrU64(int pos, const bbU64* pArr, bbUINT count) { return bbJsonArrInsArrU64(this, pos, pArr, count); }

    inline bbJsonVal& operator[](const bbCHAR* key) { bbJsonVal* v = bbJsonObjGet(this, key); return v?*v:*bbJsonObjAdd(this, key, NULL); }
    inline const bbJsonVal& operator[](const bbCHAR* key) const { return *bbJsonObjGet(this, key); }
    inline const bbJsonVal& operator[](bbUINT idx) const { return u.array.values[idx]; }

    #endif
};

#ifdef  __cplusplus
}
#endif

#endif

