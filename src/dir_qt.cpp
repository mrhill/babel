#include "dir.h"
#include "mem.h"
#include "str.h"

#if bbOS == bbOS_QT

#include <QDir>
#include <QStringList>

struct DirHandle
{
    QStringList* pEntries;
    int pos;

    DirHandle() : pEntries(NULL), pos(0) {}
    ~DirHandle() { delete pEntries; }
};

extern "C" bbDIRH bbDirOpen(const bbCHAR* pPath)
{
    QString path(pPath);
    QDir dir(path.endsWith("/*") ? path.left(path.size()-2) : path);

    if (!dir.exists())
    {
        bbErrSet(bbEFILENOTFOUND);
        return bbDIRH_INVALID;
    }

    DirHandle* pHandle = new DirHandle;

    if (!pHandle)
        bbErrSet(bbENOMEM);
    else
        pHandle->pEntries = new QStringList(dir.entryList());

    return pHandle;
}

extern "C" void bbDirClose(bbDIRH const handle)
{
    delete (DirHandle*)handle;
}

extern "C" bbERR bbDirNext(bbDIRH const handle, bbCHAR** const ppEntry, bbUINT* const pFlags)
{
    DirHandle* pHandle = (DirHandle*)handle;
    if (pHandle->pos >= pHandle->pEntries->size())
    {
        if (ppEntry) *ppEntry = NULL;
        return bbErrSet(bbEEND);
    }

    QString entry = pHandle->pEntries->at(pHandle->pos++);

    if (pFlags)
    {
        QFileInfo info(entry);
        bbUINT flags = 0;
        if (info.isDir())    flags |= bbDIRENTRY_DIR;
        if (info.isHidden()) flags |= bbDIRENTRY_SYSTEM;
        *pFlags = flags;
    }

    if (ppEntry)
        *ppEntry = bbStrFromQt(entry);

    return bbEOK;
}

#endif
