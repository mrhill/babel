#ifndef bbBABEL_HEAP_H_
#define bbBABEL_HEAP_H_

/** @file
    Heap manager
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** @addtogroup groupHeap Heap manager.
@{*/

/** Invalid heap block handle. @see bbHEAPH */
#define bbHEAPH_INVALID ((bbHEAPH)-1)

typedef struct
{
    void*  ptr;
    bbUINT size;

} bbHeapBlock;

/** @internal */
extern bbHeapBlock* bbgpHeapIndex;

/** Test if heap handle is valid.
    @retval !=0 Valid
    @retval 0 Invalid
*/
#define bbHeapIsValid(heaph) ((heaph)!=bbHEAPH_INVALID)

/** Get pointer to a heap block.
    The heap block will not be locked in memory, ie. any call
    to a heap manager function can invalidate the returned pointer.
    @param heaph (bbHEAPH) Handle of heap block
    @return Pointer to start of heap block
*/
#define bbHeapGetPtr(heaph) bbgpHeapIndex[heaph].ptr

/** Get size of a heap block.
    The heap block will not be locked in memory, ie. any call
    to a heap manager function can invalidate the returned pointer.
    @param heaph (bbHEAPH) Handle of heap block
    @return Size of heap block
*/
#define bbHeapGetSize(heaph) bbgpHeapIndex[heaph].size

/** Initialize heap manager.
    @param overallsize Overall size of heap in bytes
    @param chunksize   Maximum size of a heap chunk in bytes
    @retval bbEOK    Success, heap manager can be used
    @retval bbENOMEM Not enough memory
*/
bbERR bbHeapInit(bbU32 overallsize, bbUINT chunksize);

/** Destroy heap manager.
*/
void bbHeapDestroy(void);

/** Allocate a heap block.
    Sets bbgErr on failure to bbENOHEAP.
    @param size Size of block in bytes
    @return Handle to block or bbHEAPH_INVALID if the heap is out of memory.
*/
bbHEAPH bbHeapAlloc(const bbU32 size);

/** Resize an existing heap block.
    Sets bbgErr on failure to bbENOHEAP.
    @param heaph Handle to heap block, will be preserved
    @param newsize New size in bytes
    @retval bbEOK Success
    @retval bbENOHEAP Heap is out of memory
*/
bbERR bbHeapRealloc(const bbHEAPH heaph, const bbU32 newsize);

/** Free a heap block.
    @param heaph Handle to heap block, must be valid.
*/
void bbHeapFree(const bbHEAPH heaph);

/*@}*/

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_HEAP_H_ */
