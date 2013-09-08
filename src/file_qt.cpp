#include "file.h"

#if bbOS == bbOS_QT

#include "mem.h"
#include "str.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QByteArray>
#include <sys/stat.h>

extern "C" bbERR bbFileStat(const bbCHAR* const pPath, bbFILESTAT* const pStat)
{
    QFileInfo i(QString::fromUtf8(pPath));

    bbMemClear(pStat, sizeof(bbFILESTAT));

    if (!i.exists())
        return bbErrSet(bbEFILENOTFOUND);

    pStat->size = i.size();

    pStat->mode = 0;
    QFile::Permissions p = i.permissions();
    if (p & QFile::ReadOwner)  pStat->mode |= bbFILESTAT_READ;
    if (p & QFile::WriteOwner) pStat->mode |= bbFILESTAT_WRITE;
    if (p & QFile::ExeOwner)  pStat->mode |= bbFILESTAT_EXEC;
    if (i.isDir())
        pStat->mode |= bbFILESTAT_DIR;
    else
        pStat->mode |= bbFILESTAT_REG;

    pStat->uid = i.ownerId();
    pStat->gid = i.groupId();

    pStat->atime = i.lastRead().toTime_t();
    pStat->mtime = i.lastModified().toTime_t();
    pStat->ctime = i.created().toTime_t();

    return bbEOK;
}

extern "C" bbFILEH bbFileOpen(const bbCHAR* pFilename, bbUINT flags)
{
    QFile* pFile = new QFile(QString::fromUtf8(pFilename));
    if (!pFile)
    {
        bbErrSet(bbENOMEM);
        return NULL;
    }

    if (flags == bbFILEOPEN_READ || ((flags & bbFILEOPEN_READWRITE) && !(flags & (bbFILEOPEN_CREATE|bbFILEOPEN_TRUNC))))
    {
        if (!pFile->exists())
        {
            bbErrSet(bbEFILENOTFOUND);
            delete pFile;
            return NULL;
        }
    }

    QIODevice::OpenMode mode = (flags & bbFILEOPEN_READWRITE) ? QIODevice::ReadWrite : QIODevice::ReadOnly;

    if (flags & bbFILEOPEN_TRUNC)
        mode |= QIODevice::Truncate;

    if (!pFile->open(mode))
    {
        bbErrSet(bbEFILEOPEN);
        delete pFile;
        return NULL;
    }

    return pFile;
}

extern "C" void bbFileClose(bbFILEH handle)
{
    delete (QFile*)handle;
}

extern "C" bbU64 bbFileExt(bbFILEH handle)
{
    QFile* pFile = (QFile*)handle;
    return pFile->size();
}

extern "C" bbERR bbFileSeek(bbFILEH handle, const bbS64 offset, const bbUINT mode)
{
    QFile* pFile = (QFile*)handle;
    if (mode == bbFILESEEK_SET)
    {
        pFile->seek(offset);
        return bbEOK;
    }
    else if (mode == bbFILESEEK_CUR)
    {
        pFile->seek(pFile->pos() + offset);
        return bbEOK;
    }
    else if (mode == bbFILESEEK_END)
    {
        pFile->seek(pFile->size() + offset);
        return bbEOK;
    }
    return bbErrSet(bbEBADPARAM);
}

extern "C" bbERR bbFileRead(bbFILEH handle, void* pBuf, const bbU32 size)
{
    QFile* pFile = (QFile*)handle;
    qint64 sizeRead = pFile->read((char*)pBuf, size);
    if (sizeRead == size)
        return bbEOK;
    if (sizeRead == 0)
        return bbErrSet(bbEEOF);
    return bbErrSet(bbEFILEREAD);
}

extern "C" bbERR bbFileWrite(bbFILEH handle, const void* pBuf, const bbU32 size)
{
    QFile* pFile = (QFile*)handle;
    if (pFile->write((const char*)pBuf, size) == size)
        return bbEOK;
    return bbErrSet(bbEFILEWRITE);
}

extern "C" bbERR bbFileTrunc(bbFILEH handle)
{
    QFile* pFile = (QFile*)handle;
    if (pFile->resize(pFile->pos()))
        return bbEOK;
    return bbErrSet(bbEFILEWRITE);
}

extern "C" bbERR bbFileDelete(const bbCHAR* const pPath)
{
    if (QFile::remove(QString::fromUtf8(pPath)))
        return bbEOK;
    return bbErrSet(bbEFILEACCESS);
}

extern "C" bbERR bbFileRename(const bbCHAR* const pPath, const bbCHAR* const pNewPath)
{
    if (QFile::rename(QString::fromUtf8(pPath), QString::fromUtf8(pNewPath)))
        return bbEOK;
    return bbErrSet(bbEFILEACCESS);
}

extern "C" bbCHAR* bbPathNorm(const bbCHAR* const pPath)
{
    QFileInfo info(QString::fromUtf8(pPath));
    QDir dir = info.absoluteDir();
    dir = QDir(dir.absolutePath());
    QByteArray norm = dir.absoluteFilePath(info.fileName()).toUtf8();
    bbCHAR* pNorm = bbStrAlloc(norm.size());
    if (pNorm)
        bbStrCpy(pNorm, norm.constData());
    return pNorm;
}

extern "C" bbCHAR* bbPathTemp(const bbCHAR* pDir)
{
    QDir dir(pDir ? QString::fromUtf8(pDir) : QDir::tempPath());

    bbU32 i = 0;
    bbU32 r = 0x2342;
    QString name;
    for(;;)
    {
        name.sprintf("%06X", r&0xFFFFFF);
        QFileInfo info(dir, name);
        if (!info.exists())
            return bbStrFromQt(info.absoluteFilePath());

        r = ((r<<3)|(r>>(32-3))) ^ r;
        if (++i == 256)
            return NULL;
    }
}

#endif
