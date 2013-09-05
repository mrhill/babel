#include "strbuf.h"
#include "mem.h"
#include "str.h"

int bbStrBuf::Printf(const bbCHAR* pFmt, ...)
{
    bbVALIST args;
    bbVASTART(args, pFmt);
    int ret = VPrintf(pFmt, args);
    bbVAEND(args);
    return ret;
}

int bbStrBuf::Catf(const bbCHAR* pFmt, ...)
{
    bbVALIST args;
    bbVASTART(args, pFmt);
    int ret = VCatf(pFmt, args);
    bbVAEND(args);
    return ret;
}

#if bbOS == bbOS_QT
#include <QByteArray>
#include <QString>
bbCHAR* bbStrFromQt(const QString& str)
{
    QByteArray utf8 = str.toUtf8();
    bbCHAR* pStr = bbStrAlloc(utf8.size());
    if (pStr)
        bbStrCpy(pStr, utf8.constData());
    return pStr;
}
#endif

