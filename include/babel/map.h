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
bbERR bbMapAdd(struct bbMap* pMap, const bbCHAR* pKey, void* val);

/** Add value-key pair to map (key string under external control).
    An existing key will be replaced.
    @param pKey Key, 0-terminated string, will not be copied
    @param val  Data associated with key
*/
bbERR bbMapAddC(struct bbMap* pMap, const bbCHAR* pKey, void* val);

/** Search map for key.
    @param pMap Map
    @param pKey Key, 0-terminated string
    @return Returns value, or NULL if not found (bbENOTFOUND).
            Call bbErrGet() to distinguish between error and NULL-value.
*/
void* bbMapGet(const struct bbMap* pMap, const bbCHAR* pKey);

void bbMapDump(const struct bbMap* pMap);

struct bbMap
{
    bbMapPair* mpPairs;
    bbMapKeyChunk* mpKeys;
    bbCmpFn    mCmpFn;
    bbUINT     mSize;
    bbUINT     mCapacity;
    bbUINT     mKeyChunkUsed;

    #ifdef  __cplusplus
    inline bbMap() { bbMapInit(this); }
    inline bbMap(bbCmpFn cmpFn) { bbMapInit(this); mCmpFn = cmpFn; }
    inline ~bbMap() { bbMapDestroy(this); }
    inline void SetCmpFn(bbCmpFn cmpFn) { mCmpFn = cmpFn; }
    inline bbERR Add(const bbCHAR* pKey, void* val) { return bbMapAdd(this, pKey, val); }
    inline bbERR AddC(const bbCHAR* pKey, void* val) { return bbMapAddC(this, pKey, val); }
    inline void* Get(const bbCHAR* pKey) const { return bbMapGet(this, pKey); }
    inline void* operator[](const bbCHAR* pKey) const { return bbMapGet(this, pKey); }
    #endif
};

int bbCmp_Str2MapPair(const void *, const void *);
int bbCmpI_Str2MapPair(const void *, const void *);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_MAP_H_ */

