#include "defs.h"
#include "str.h"

bbERR  bbgErr;

bbCHAR bbgErrStr[bbERRSTRMAXLEN];

const bbCHAR* bbGetErrStr(const bbERR err)
{
    switch(err)
    {
    case bbEUK       : bbStrCpy(bbgErrStr, bbT("Unknown")); break;
    case bbENOMEM    : bbStrCpy(bbgErrStr, bbT("Out of memory")); break;
    case bbENOTFOUND : bbStrCpy(bbgErrStr, bbT("Not found")); break;
    case bbENOTSUP   : bbStrCpy(bbgErrStr, bbT("Operation not supported")); break;
    case bbEBADPARAM : bbStrCpy(bbgErrStr, bbT("Bad parameter")); break;
    case bbEBADSTATE : bbStrCpy(bbgErrStr, bbT("Bad state")); break;
    case bbESYNTAX   : bbStrCpy(bbgErrStr, bbT("Syntax error")); break;
    case bbEOVERFLOW : bbStrCpy(bbgErrStr, bbT("Overflow")); break;
    case bbESTR:
    case bbEJSONSYNTAX : break;
    default: bbSprintf(bbgErrStr, "Error %d", err); break;
    }
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