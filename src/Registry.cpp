#include "Registry.h"

int bbRegistry::NextID()
{
    int id;
    for (id=0; id<(int)mEntries.GetSize(); id++)
    {
        if (mEntries[id].pObj == NULL)
            break;
    }
    return id;
}

int bbRegistry::Add(void* const pObj)
{
    int id = NextID();

    if ((unsigned)id == mEntries.GetSize())
    {
        if (!mEntries.Grow(1))
            return -1;
    }

    mEntries[id].pObj = pObj;
    return id;
}

bbUINT bbRegistry::GetCount() const
{
    bbUINT count = 0;
    const bbRegistryObject* p = mEntries.GetPtr();
    const bbRegistryObject* const p_end = mEntries.GetPtrEnd();

    while (p < p_end)
    {
        if (p->pObj)
            count++;
        p++;
    }

    return count;
}

int bbRegistry::Find(void* const pObj)
{
    int i = mEntries.GetSize();
    for(;;)
    {
        if (--i < 0) break;
        if (mEntries[i].pObj == pObj) break;
    }
    return i;
}

