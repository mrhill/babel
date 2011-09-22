#ifndef bbBABEL_MAP_H_
#define bbBABEL_MAP_H_

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct
{
    const bbCHAR* pKey;
    void*   val;
    bbU8    alloced;

} bbMapPair;

typedef struct
{
    bbMapPair* pKeys;
    bbUINT size;

} bbMap;

void bbMapInit(bbMap* const pMap);
void bbMapDestroy(bbMap* const pMap);

/** Add value-key pair to map.
    @param pKey Key, 0-terminated string, will be copied
    @param val  Data associated with key
*/
bbERR bbMapAdd(bbMap* const pMap, const bbCHAR* pKey, void* val);

/** Add value-key pair to map (key string under external control).
    @param pKey Key, 0-terminated string, will not be copied
    @param val  Data associated with key
*/
bbERR bbMapAddC(bbMap* const pMap, const bbCHAR* pKey, void* val);

/** Search map for key.
    @param pMap Map
    @param pKey Key, 0-terminated string
    @return Returns value, or NULL if not found (bbENOTFOUND).
            Call bbErrGet() to distinguish between error and NULL-value.
*/
void* bbMapGet(bbMap* const pMap, bbCHAR* pKey);

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_MAP_H_ */

