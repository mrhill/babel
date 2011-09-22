#include "defs.h"

bbERR  bbgErr;

bbCHAR bbgErrStr[bbERRSTRMAXLEN];

const bbCHAR* bbGetErrStr(const bbERR err)
{
    bbgErrStr[0] = 0;
    return bbgErrStr;
}

#ifdef bbDEBUG

#define bbDEBUGMUTEXMAX 8
static bbU32  gDebugMutexId[bbDEBUGMUTEXMAX]   = { 0,0,0,0,0,0,0,0 };

void bbDEBUGMUTEX_LOCK(bbU32 id)
{
    int i, j=-1;
    for(i=0; i<bbDEBUGMUTEXMAX; i++)
    {
        if ((j==-1) && (gDebugMutexId[i]==0)) j=i;
        bbASSERT(gDebugMutexId[i] != id); // same mutex locked twice
    }
    bbASSERT(j!=-1); // maximum number of mutexes taken, adjust code
    gDebugMutexId[j] = id;
}

void bbDEBUGMUTEX_UNLOCK(bbU32 id)
{
    int i;
    for(i=0; i<bbDEBUGMUTEXMAX; i++)
    {
        if (gDebugMutexId[i] == id)
        {
            gDebugMutexId[i] = 0;
            return;
        }
    }
    bbASSERT(0); // mutex not found
}

#endif // bbDEBUG