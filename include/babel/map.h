#ifndef bbBABEL_MAP_H_
#define bbBABEL_MAP_H_

#include "defs.h"
#include "mem.h"
#include "algo.h"

struct bbMapPair;
struct bbMapKeyChunk;
struct bbMap;

#ifdef  __cplusplus
extern "C" {
#else
typedef struct bbMapPair bbMapPair;
typedef struct bbMapKeyChunk bbMapKeyChunk;
typedef struct bbMap bbMap;
#endif

/** Construct a map.
    Only needed when used via C interface.
    @param pMap Map instance to be initialized, struct is under external control.
*/
void bbMapInit(struct bbMap* pMap);

/** Destroy a map.
    Only needed when used via C interface.
    @param pMap Map instance to destroy, destroys all keys and values, struct stays under external control.
*/
void bbMapDestroy(struct bbMap* pMap);

/** Add value-key pair to map.
    An existing key will be replaced.
    @param pKey Key, 0-terminated string, will be copied
    @param val  Data associated with key
*/
bbERR bbMapAdd(struct bbMap* pMap, const bbCHAR* pKey, bbU64PTR val);

/** Add value-key pair to map (key string under external control).
    An existing key will be replaced.
    @param pKey Key, 0-terminated string, will not be copied
    @param val  Data associated with key
*/
bbERR bbMapAddC(struct bbMap* pMap, const bbCHAR* pKey, bbU64PTR val);

/** Search map for key.
    @param pMap Map
    @param pKey Key, 0-terminated string
    @return Returns value, or 0 if not found (bbENOTFOUND).
            Call bbErrGet() to distinguish between error and NULL-value.
*/
bbU64PTR bbMapGet(const struct bbMap* pMap, const bbCHAR* pKey);

/** Return number of pairs in map.
    @param pMap Map
    @return Number of entries
*/
#define bbMapGetSize(pMap) ((pMap)->mSize)

/** Enumerate all entries in map.
    @param pMap Map
    @param cb   Callback function to process entries.
                Key and value are passed as 1st and 2nd parameter, return !=0 to stop enumeration.
*/
void bbMapEnumerate(const struct bbMap* pMap, int (*cb)(const bbCHAR*, bbU64PTR));

/** Debug dump map to stdout
    @param pMap Map
*/
void bbMapDump(const struct bbMap* pMap);

struct bbMap
{
    bbMapPair* mpPairs;
    bbMapKeyChunk* mpKeys;
    bbCmpFn    mCmpFn;
    bbUINT     mSize;
    bbUINT     mKeyChunkUsed;

    #ifdef  __cplusplus
    inline bbMap() { bbMapInit(this); }
    inline bbMap(bbCmpFn cmpFn) { bbMapInit(this); mCmpFn = cmpFn; }
    inline ~bbMap() { bbMapDestroy(this); }
    inline void SetCmpFn(bbCmpFn cmpFn) { mCmpFn = cmpFn; }
    inline bbERR Add(const bbCHAR* pKey, bbU64PTR val) { return bbMapAdd(this, pKey, val); }
    inline bbERR Add(const bbCHAR* pKey, void* val) { return bbMapAdd(this, pKey, (bbU64PTR)val); }
    inline bbERR AddC(const bbCHAR* pKey, bbU64PTR val) { return bbMapAddC(this, pKey, val); }
    inline bbERR AddC(const bbCHAR* pKey, void* val) { return bbMapAddC(this, pKey, (bbU64PTR)val); }
    inline bbU64PTR Get(const bbCHAR* pKey) const { return bbMapGet(this, pKey); }
    inline bbU64PTR operator[](const bbCHAR* pKey) const { return bbMapGet(this, pKey); }
    inline bbUINT GetSize() const { return mSize; }
    inline void Enumerate(int (*cb)(const bbCHAR*, bbU64PTR)) { bbMapEnumerate(this, cb); }
    #endif
};

int bbCmp_Str2MapPair(const void *, const void *);
int bbCmpI_Str2MapPair(const void *, const void *);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_MAP_H_ */

