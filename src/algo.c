#include "algo.h"

int bbCmp_bbU32(const void * p1, const void * p2)
{
    const bbU32 v1 = *((bbU32*)p1);
    const bbU32 v2 = *((bbU32*)p2);
    if (v1 < v2) return -1;
    if (v1 > v2) return 1;
    return 0;
}

int bbCmp_bbU8(const void * p1, const void * p2)
{
    const bbU8 v1 = *((bbU8*)p1);
    const bbU8 v2 = *((bbU8*)p2);
    if (v1 < v2) return -1;
    if (v1 > v2) return 1;
    return 0;
}

