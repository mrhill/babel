#ifndef bbBABEL_MEM_H_
#define bbBABEL_MEM_H_

/** @file
	Memory management.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** @addtogroup groupMem Memory management
@{*/

/** Fill a block of memory.
    Fills specifyed amount of bytes with a value.
    @param pDst (void*) Destination address
    @param c    (bbU8)  Value
    @param size (bbUINT) Number of bytes to initialize
    @return Returns \a pDst
*/
#if bbOS == bbOS_PALMOS
#define bbMemSet(pDst, c, size) MemSet(pDst, size, c)
#else
#define bbMemSet(pDst, c, size) memset(pDst, c, size)
#endif

/** Fill a block of memory with 0 bytes.
    Fills specifyed amount of bytes with 0.
    @param pDst (void*) Destination address
    @param size (bbUINT) Number of bytes to initialize
*/
#define bbMemClear(pDst, size) bbMemSet(pDst, 0, size);

/** Copy a block in memory.
    Copies specifyed amount of bytes from one location
    to another. The two areas may overlap.
    @param pDst (void*) Destination address
    @param pSrc (void*) Source address
    @param size (bbUINT) Number of bytes to copy
    @return Returns \a pDst
*/
#if bbOS == bbOS_PALMOS
#define bbMemMove(pDst, pSrc, size) MemMove(pDst, pSrc, size)
#else
#define bbMemMove(pDst, pSrc, size) memmove(pDst, pSrc, size)
#endif

/** Copy a block in memory.
    Copies specifyed amount of bytes from one location
    to another. The two areas must not overlap.
    @param pDst (void*) Destination address
    @param pSrc (void*) Source address
    @param size (bbUINT) Number of bytes to copy
    @return Returns \a pDst
*/
#define bbMemCpy(pDst, pSrc, size) memcpy(pDst, pSrc, size)

/** Byte-wise compare of two memory blocks.
    @param p1 (void*) Block 1
    @param p2 (void*) Block 2
    @param size (bbUINT) Number of bytes to compare
    @retval <0 p1 < p2
    @retval  0 p1 == p2
    @retval >0 p1 > p2
*/
#if bbOS == bbOS_PALMOS
#define bbMemCmp(p1, p2, size) MemCmp(p1, p2, size)
#else
#define bbMemCmp(p1, p2, size) memcmp(p1, p2, size)
#endif

/** Allocate a block from the heap.
    Sets bbgErr on failure.
    @param size Block size in bytes
    @return Pointer to block block pointer, or NULL on failure
*/
void* bbMemAlloc(const bbU32 size);

/** Allocate a block from the heap and initialize to 0.
    Sets bbgErr on failure.
    @param size Block size in bytes
    @return Pointer to block block pointer, or NULL on failure
*/
void* bbMemCalloc(const bbU32 size);

/** Allocate heap block large enough to hold string of specified length + 0-terminator.
    @param strlen (bbUINT) String length in character code units.
    @return (bbCHAR*) Pointer to heap block, or NULL on error. Block must be freed later using bbMemFree()
*/
#define bbStrAlloc(strlen) ((bbCHAR*)bbMemAlloc(((strlen)+1)*sizeof(bbCHAR)))

/** Allocate, reallocate or free a block from the C heap
    If \a size is 0, a possibly allocated block will be freed, \a *pPtr will be set to NULL.
    If \a size is greater than the current blocksize, the block might get moved
    in memory address space, \a *pPtr is updated accordingly. Shrinking and freeing always
    succeeds. \a *pPtr and the block's contents are left unchanged on failure.
    Sets bbgErr on failure.
    @param size Block size in bytes
    @param pPtr Pointer to block pointer, will be updated on success
    @return bbEOK on success, or value of #bbgErr on failure
    @retval bbEOK    Success
    @retval bbENOMEM Out of heap memory
*/
bbERR bbMemRealloc(const bbU32 size, void** pPtr);

/** Free a block claimed from the C heap.
    The block pointed to by \a ptr is freed.
    @param ptr (void*) Pointer to start of heap block, can be NULL
    @return (void)
*/
#if bbOS == bbOS_PALMOS
#define bbMemFree(ptr) MemPtrFree(ptr)
#else
#define bbMemFree(ptr) free(ptr)
#endif

/** Free a block claimed from the C heap and set pointer to NULL.
    The block pointed to by \a *pPtr is freed and \a *pPtr is set to NULL.
    If \a *pPtr is NULL on entry, the call has no effect.
    @param pPtr Pointer to heap block pointer.
*/
void bbMemFreeNull(void** pPtr);

/** Swap contents of memory blocks.
    @param p1 1st block
    @param p2 2nd block
    @param size Size of data to swap in bytes
*/
void bbMemSwap(void* p1, void* p2, bbU32 size);

/*@}*/

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_MEM_H_ */
