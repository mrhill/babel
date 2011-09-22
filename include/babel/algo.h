/* $Id: algo.h,v 1.4 2009/03/15 10:52:06 mrhill Exp $ */

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
    @param cmpfn     (int (*compar)(const void *, const void *)) Compare function
    @return (void*) Pointer to match or NULL if no match
*/
#define bbBSearch(pKey, pBase, arraysize, itemsize, cmpfn) bsearch(pKey, pBase, arraysize, itemsize, cmpfn)

/** Compare function for type bbU32. */
int bbCmp_bbU32(const void *, const void *);

/** Compare function for type bbU8. */
int bbCmp_bbU8(const void *, const void *);

/*@}*/

#ifdef  __cplusplus
}
#endif

#endif /* bbBABEL_TIME_H_ */
