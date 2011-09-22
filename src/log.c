#include "log.h"
#include "str.h"
#ifdef ANDROID
#include <android/log.h> 
#endif

static bbLOGHANDLER g_loghandler = bbLogDefaultHandler;
static bbU8 g_loglevelmask[bbNumLogLevels] = { 1,1,1,1,1,1,1,1 }; // bytemask is smaller codesize than bitmask

bbLOGHANDLER bbLogSetHandler(bbLOGHANDLER loghandler)
{
    bbLOGHANDLER const old_loghandler = g_loghandler;
    g_loghandler = loghandler;
    return old_loghandler;
}

bbUINT bbLogLevelEnable(const bbLOGLEVEL loglevel, const bbUINT enable)
{
    const bbUINT oldstate = g_loglevelmask[loglevel];
    g_loglevelmask[loglevel] = (bbU8) enable;
    return oldstate;
}

void bbLog(const bbLOGLEVEL loglevel, const bbCHAR* pFmt, ...)
{
    bbVALIST args;
    bbVASTART(args, pFmt);
    bbLogV( loglevel, pFmt, args );
    bbVAEND(args);
}

void bbLogV(const bbLOGLEVEL loglevel, const bbCHAR* pFmt, bbVALIST args)
{
    if (g_loglevelmask[loglevel] && g_loghandler)
    {
        bbVsnprintf(bbgErrStr, bbERRSTRMAXLEN, pFmt, args);
        g_loghandler(loglevel, bbgErrStr);
    }
}

void bbLogDefaultHandler(const bbLOGLEVEL loglevel, const bbCHAR* pMsg)
{
#ifdef ANDROID
    __android_log_print(ANDROID_LOG_DEBUG, __FILE__, pMsg);
#else
    if( loglevel == bbErr )
    {
        bbPrintf( bbT("Error: %s"), pMsg );
    }
    else if( loglevel == bbWarning )
    {
        bbPrintf( bbT("Warning: %s"), pMsg );
    }
    else
    {
        bbPrintf( bbT("%s"), pMsg );
    }
#endif
}
