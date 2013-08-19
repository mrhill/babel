#ifndef bbBABEL_MAP_H_
#define bbBABEL_MAP_H_

#include "defs.h"
#include "mem.h"

struct bbMapKeyChunk;

typedef struct bbMapPair
{
    const bbCHAR* key;
    bbU64PTR val;

} bbMapPair;

struct bbMapRec
{
    struct bbMapPair*     mpPairs;
    struct bbMapKeyChunk* mpKeyChunk;
    bbUINT                mSize;
};

#ifdef  __cplusplus
extern "C" {
#else
typedef struct bbMapRec bbMapRec;
typedef bbMapRec bbMap;
#endif


/** Construct a map.
    Only needed when used via C interface.
    @param pMap Map instance to be initialized, struct is under external control.
*/
void bbMapInit(bbMapRec* pMap);

/** Destroy a map.
    Only needed when used via C interface.
    @param pMap Map instance to destroy, destroys all keys and values, struct stays under external control.
*/
void bbMapDestroy(bbMapRec* pMap);

/** Copy a map. */
bbERR bbMapInitCopy(bbMapRec* pDst, const bbMapRec* pSrc);

/** Add value-key pair to map.
    An existing key will be replaced.
    @param pKey Key, 0-terminated string, will be copied
    @param val  Data associated with key
    @return Insert index, or -1 on failure
*/
int bbMapAdd(bbMapRec* pMap, const bbCHAR* pKey, bbU64PTR val);

/** Add value-key pair to map (key string under external control).
    An existing key will be replaced.
    @param pKey Key, 0-terminated string, will not be copied
    @param val  Data associated with key
    @return Insert index, or -1 on failure
*/
int bbMapAddC(bbMapRec* pMap, const bbCHAR* pKey, bbU64PTR val);

/** Search map for key.
    @param pMap Map
    @param pKey Key, 0-terminated string, can be NULL to force failure.
    @return Returns value, or 0 if not found (bbENOTFOUND).
            Call bbErrGet() to distinguish between error and NULL-value.
*/
bbU64PTR bbMapGet(const bbMapRec* pMap, const bbCHAR* pKey);

/** Get value from map by index.
    @param pMap (const struct bbMap*) Map
    @param index (bbUINT) Element index, must be within 0 and bbMapGetSize()
    @return (const bbMapPair*) Value-key pair, always succeeds.
*/
#define bbMapGetPair(pMap, index) ((pMap)->mpPairs + (index))

/** Remove a key-value pair and return the previously stored value.
    @param pMap Map
    @param pKey Key, 0-terminated string, can be NULL to force failure.
    @return Returns value, or 0 if not found (bbENOTFOUND).
*/
bbU64PTR bbMapDel(bbMapRec* pMap, const bbCHAR* pKey);

/** Remove a key-value pair by index and return the previously stored value.
    @param pMap Map
    @param index (bbUINT) Element index, must be within 0 and bbMapGetSize()
    @return Returns value.
*/
bbU64PTR bbMapDelIndex(bbMapRec* pMap, bbUINT index);

/** Return number of pairs in map.
    @param pMap (const struct bbMap*) Map
    @return (bbUINT) Number of entries
*/
#define bbMapGetSize(pMap) ((pMap)->mSize)

/** Enumerate all entries in map.
    @param pMap Map
    @param cb   Callback function to process entries.
                Key and value are passed as 1st and 2nd parameter, return error to stop enumeration.
    @param user User context to pass into callback
*/
bbERR bbMapEnumerate(const bbMapRec* pMap, bbERR (*cb)(const bbCHAR*, bbU64PTR, void*), void* user);

/** Debug dump map to stdout
    @param pMap Map
*/
void bbMapDump(const bbMapRec* pMap);

/** Key-value map.

    Keys are 0-terminated strings, and values are bbU64PTR. Maximum key length is 65535, including terminator.

    If pairs are added via Add(), the key string is copied to internal storage.
    If paris are added via AddC(), the original key string pointer is stored.
    If AddC() is used, the key pointer must remain valid for the lifetime of the bbMap instance,
    even if the same key is added with Add() later.

    Keys are stored in a sorted array, insert time is O(N + logN), look-up time is O(logN), and
    delete time is O(N + logN).
*/
#ifdef __cplusplus
struct bbMap : bbMapRec
{
    inline bbMap() { bbMapInit(this); }
    inline ~bbMap() { bbMapDestroy(this); }
    inline int Add(const bbCHAR* pKey, bbU64PTR val) { return bbMapAdd(this, pKey, val); }
    inline int Add(const bbCHAR* pKey, void* val) { return bbMapAdd(this, pKey, (bbU64PTR)val); }
    inline int AddC(const bbCHAR* pKey, bbU64PTR val) { return bbMapAddC(this, pKey, val); }
    inline int AddC(const bbCHAR* pKey, void* val) { return bbMapAddC(this, pKey, (bbU64PTR)val); }
    inline bbU64PTR Get(const bbCHAR* pKey) const { return bbMapGet(this, pKey); }
    inline bbU8 GetU8(const bbCHAR* pKey) const { return (bbU8)bbMapGet(this, pKey); }
    inline bbU16 GetU16(const bbCHAR* pKey) const { return (bbU16)bbMapGet(this, pKey); }
    inline bbU32 GetU32(const bbCHAR* pKey) const { return (bbU32)bbMapGet(this, pKey); }
    inline bbU64 GetU64(const bbCHAR* pKey) const { return (bbU64)bbMapGet(this, pKey); }
    inline const bbCHAR* GetStr(const bbCHAR* pKey) const { return (const bbCHAR*)bbMapGet(this, pKey); }
    inline const void* GetPtr(const bbCHAR* pKey) const { return (const void*)bbMapGet(this, pKey); }
    inline const bbMapPair* GetPair(bbUINT index) const { return bbMapGetPair(this, index); }
    inline bbU64PTR operator[](const bbCHAR* pKey) const { return bbMapGet(this, pKey); }
    inline bbU64PTR Del(const bbCHAR* pKey) { return bbMapDel(this, pKey); }
    inline bbUINT GetSize() const { return mSize; }
    inline bbERR Enumerate(bbERR (*cb)(const bbCHAR*, bbU64PTR, void*), void* user) { return bbMapEnumerate(this, cb, user); }
};

}
#endif

#endif /* bbBABEL_MAP_H_ */

