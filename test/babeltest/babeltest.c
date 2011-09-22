#include <babel/log.h>
#include <babel/str.h>
#include <babel/mem.h>
#include <babel/fixmath.h>
#include <babel/timedate.h>
#include <babel/dir.h>

#include <locale.h>

static bbLOGHANDLER g_old_loghandler;

void TestLogHandler(bbLOGLEVEL const loglevel, const bbCHAR* pMsg)
{
    bbCHAR* pLogLevelStr = bbT("");

    switch (loglevel)
    {
    case bbErr:     pLogLevelStr = bbT("bbErr");     break;
    case bbWarning: pLogLevelStr = bbT("bbWarning"); break;
    case bbMsg:     pLogLevelStr = bbT("bbMsg");     break;
    case bbStatus:  pLogLevelStr = bbT("bbStatus");  break;
    case bbVerbose: pLogLevelStr = bbT("bbVerbose"); break;
    case bbDebug:   pLogLevelStr = bbT("bbDebug");   break;
    case bbUser:    pLogLevelStr = bbT("bbUser");    break;
    }

    bbPrintf( bbT("%s: %s\n"), pLogLevelStr, pMsg);
}

void testlog(void)
{
    g_old_loghandler = bbLogSetHandler( TestLogHandler );

    bbLog( bbDebug, bbT("Debug %s"), bbT("pffff") );
    bbLog( bbDebug, bbT("Š¿Žš"));

    bbLog( bbWarning, bbT("Warning 1!"));
    bbLogLevelEnable( bbWarning, 0);
    bbLog( bbWarning, bbT("Warning 2!"));
    bbLogLevelEnable( bbWarning, 1);
    bbLog( bbWarning, bbT("Warning 3!"));
}

void testerr(void)
{
    bbErrSet( 23);
    bbLog( bbMsg, bbT("Last error: %d"), bbErrGet());
}

void teststr(void)
{
    bbCHAR tmp[100];

    bbLog( bbMsg, bbT("strlen \"abc DEF\" = %d"), bbStrLen(bbT("abc DEF")));
    bbLog( bbMsg, bbT("strlen \"Š¿Žš\" = %d"), bbStrLen(bbT("Š¿Žš")));
    bbLog( bbMsg, bbT("strcmp abc DEF = %d"), bbStrCmp(bbT("abc"), bbT("DEF")));
    bbLog( bbMsg, bbT("stricmp abc DEF = %d"), bbStrICmp(bbT("abc"), bbT("DEF")));

    bbStrCpy( tmp, bbT("AHAHA"));
    bbLog( bbMsg, bbT("strcpy(dst, \"AHAHA\") -> %s"), tmp);
}

void dumpmem(bbU8* ptr, bbUINT size)
{
    bbUINT i = 0;
    while (size-->0)
    {
        bbPrintf(bbT("%2X "), ptr[i++]);
        if (!(i & 7)) bbPrintf(bbT("\n"));
    }
}

void testmem(void)
{
    bbUINT size = 256;
    bbU8 *ptr1, *ptr2;
    bbERR err;

    ptr1 = (bbU8*)bbMemAlloc(size); err = bbErrGet();
    bbLog( bbMsg, bbT("Claimed %d bytes at %p, error %d"), size, ptr1, err);

    size = 1024;
    ptr2 = (bbU8*)bbMemAlloc(size); err = bbErrGet();
    bbLog( bbMsg, bbT("Claimed %d bytes at %p, error %d"), size, ptr2, err);

    bbMemFree(ptr1);
    bbLog( bbMsg, bbT("Free block 1, ptr is now %p"), ptr1);

    size = 2048;
    err = bbMemRealloc(size, (void**)&ptr2);
    bbLog( bbMsg, bbT("Realloced ptr2 to %d bytes at %p, error %d"), size, ptr2, err);

    bbMemSet(ptr2, 0x23, 16);
    bbLog( bbMsg, bbT("bbMemSet(ptr2, 0x23, 16)"));
    dumpmem(ptr2, 32);
    bbMemMove(ptr2 + 8, ptr2, 16);
    bbMemMove(ptr2, ptr2 + 24, 8);
    bbLog( bbMsg, bbT("bbMemMove'ing a bit"));
    dumpmem(ptr2, 32);

    bbMemFree(ptr2);
}

void testfixmath(void)
{
    bbU32 v;
    bbU32 rnd = 0x2398E375;
    bbUINT i;

    bbLog( bbMsg, bbT("bbCountSetBits(0x%08lx) = %d"), rnd, bbCountSetBits(rnd));
    bbLog( bbMsg, bbT("bbCountSetBits(0x%08lx) = %d"), rnd>>16, bbCountSetBits(rnd>>16));

    v = 0UL;  bbLog( bbMsg, bbT("bbGetTopBit(0x%08lx) = %d"), v, bbGetTopBit(v));
    v = ~0UL; bbLog( bbMsg, bbT("bbGetTopBit(0x%08lx) = %d"), v, bbGetTopBit(v));

    for (i=0; i<10; ++i)
    {
        rnd = rnd ^ (rnd>>13) ^ (rnd<<(32-13));

        v = rnd;     bbLog( bbMsg, bbT("bbGetTopBit(0x%08lx) = %d"), v, bbGetTopBit(v));
        v = rnd>>16; bbLog( bbMsg, bbT("bbGetTopBit(0x%08lx) = %d"), v, bbGetTopBit(v));
    }
}

void testtime(void)
{
    bbU32 wait = 200;
    bbU32 tickend = bbGetTickCount() + wait;
    bbLog( bbMsg, bbT("bbGetTickCount() = %ld, waiting %ld ms ..."), bbGetTickCount(), wait);
    while (bbGetTickCount() < tickend);

    bbLog( bbMsg, bbT("bbGetTime() = %ld"), bbGetTime());
}

void testdir(void)
{
    bbERR  err;
    bbDIRH dirh;
    bbCHAR* pDirStr = bbT("*"); 

    dirh = bbDirOpen( pDirStr); err = bbErrGet();
    bbLog( bbMsg, bbT("bbDirOpen of '%s', error %d"), pDirStr, err);

    if (err == bbEOK)
    {
        bbCHAR* pPath;
        bbUINT  flags;

        do
        {
            err = bbDirNext( dirh, &pPath, &flags);
            bbLog( bbMsg, bbT("bbDirNext, error %d"), err);

            if (err == bbEOK)
            {
                bbLog( bbMsg, bbT(" entry: '%s', flags=%x"), pPath, flags);
                bbMemFree(pPath);
            }

        } while (err == bbEOK);

        bbDirClose(dirh);
    }
}

void testunicode(void)
{
    int i;
    bbCHARCP cp;
    bbU32 size;
    bbUINT block;

    static bbCHARCP cps[] = { 
        0, 0x79, 0x80, 0xFF, 0x7BF, 0x7CF, 0x900, 0xE01EF, 0xF0000, 0x100000, 0x10FFFF, 0x110000
    };

    static bbUINT ranges[] = { 0, 1, 166, 167, 165 };

    static bbCHAR* blocknames[] = { bbCPGUNICODE_BLOCKNAMES } ;

    for (i=0; i<sizeof(cps)/sizeof(bbCHARCP); i++)
    {
        cp = cps[i];
        block = bbCpgUnicode_GetBlock(cp);
        bbPrintf(bbT("cp=0x%06X -> block %d "), cp, block);
        bbLog(bbMsg, (block!=(bbUINT)-1) ? blocknames[block] : bbT(""));
    }

    for (i=0; i<sizeof(ranges)/sizeof(bbUINT); i++)    
    {
        cp = bbCpgUnicode_GetBlockRange(ranges[i], &size);
        bbPrintf(bbT("%d -> 0x%X:%d "), ranges[i], cp, size);
        bbLog(bbMsg, blocknames[ranges[i]]);
    }
}

void testfile(void)
{
    //bbFILEH hFile;
    //xxx
}

int main(int argc, char** argv)
{
    char c = (char)255;
    int ic = (int) c;
    wchar_t wc = (wchar_t)65535;
    int iwc = (int) wc;

    bbLog( bbMsg, bbT("%s started\n"), argv[0]);

    #if bbSIZEOF_CHAR == 1
    bbLog( bbMsg, bbT("ANSI locale: %s\n"), setlocale( LC_ALL, NULL ));
    #else
    bbLog( bbMsg, bbT("ANSI locale: %s\n"), _wsetlocale( LC_ALL, NULL ));
    #endif

    bbLog( bbMsg, bbT("sizeof(wchar_t): %d\n"), sizeof(wchar_t));
    bbLog( bbMsg, bbT("Signed wchar_t: %s\n"), (iwc>0)?bbT("no"):bbT("yes"));
    bbLog( bbMsg, bbT("Signed char: %s\n"), (ic>0)?bbT("no"):bbT("yes"));

    bbPrintf( bbT("testfile------------------\n")); 
    testfile();
    bbPrintf( bbT("testlog-------------------\n")); 
    testlog();
    bbPrintf( bbT("testerr-------------------\n")); 
    testerr();
    bbPrintf( bbT("teststr-------------------\n")); 
    teststr();
    bbPrintf( bbT("testmem-------------------\n")); 
    testmem();
    bbPrintf( bbT("testfixmath---------------\n")); 
    testfixmath();
    bbPrintf( bbT("testtime------------------\n")); 
    testtime();
    bbPrintf( bbT("testdir-------------------\n")); 
    testdir();
    bbPrintf( bbT("testunicode---------------\n")); 
    testunicode();

    return 0;
}
