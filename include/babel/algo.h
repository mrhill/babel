#ifndef bbBABEL_ALGO_H_
#define bbBABEL_ALGO_H_

/** @file
	Algorithms.
*/

#include "defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

/** @addtogroup groupAlgo Algorithms
@{*/

typedef int (*bbCmpFn)(const void *, const void *);

/** Quicksort.
    @param pBase     (void*) Pointer to start of item array.
    @param arraysize (bbU32) Size of array in number of elements
    @param itemsize  (bbUINT) Size of an item in bytes
    @param cmpfn     (int (*compar)(const void *, const void *)) Compare function
*/
#define bbQSort(pBase, arraysize, itemsize, cmpfn) qsort(pBase, arraysize, itemsize, cmpfn)

/** Binary search.
    @param pKey      (const void*) Pointer to search key.
    @param pBase     (const void*) Pointer to start of item array.
    @param arraysize (bbU32) Size of array in number of elements
    @param itemsize  (bbUINT) Size of an item in bytes
    @param cmpfn     (int (*cmpfn)(const void *, const void *)) Compare function.
                     When \a cmpfn is called, first parameter points to the key to insert,
                     and the second to an existing element in the array.
    @return (void*) Pointer to match or NULL if no match
*/
#define bbBSearch(pKey, pBase, arraysize, itemsize, cmpfn) bsearch(pKey, pBase, arraysize, itemsize, cmpfn)

/** Binary search for element greater or equal.
    Like \a bbBSearch but returns an element from the array, which is greater or equal to the key,
    i.e. it is a valid insertion point for the key to maintain order.
    If all elements are smaller, or the array is empty, the end of the array is returned.
*/
void* bbBSearchGE(const void* pKey, const void* pBase, bbUINT nmemb, bbUINT size, bbCmpFn cmpFn);

/** Compare function for type bbU32. */
int bbCmp_bbU32(const void *, const void *);

/** Compare function for type bbU8. */
int bbCmp_bbU8(const void *, const void *);

/*@}*/

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_TIME_H_ */
