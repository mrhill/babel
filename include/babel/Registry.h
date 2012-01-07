#ifndef bbRegistry_H_
#define bbRegistry_H_

#include "Arr.h"

/** Registry object container, helper for bbRegistry. */
struct bbRegistryObject
{
    void* pObj;
};

#if bbSIZEOF_UPTR == 8
bbDECLAREARR(bbRegistryObject, bbArrRegistryObject, 8);
#else
bbDECLAREARR(bbRegistryObject, bbArrRegistryObject, 4);
#endif

/** Object registry. */
struct bbRegistry
{
    bbArrRegistryObject mEntries;

    ~bbRegistry()
    {
        bbASSERT(GetCount() == 0);
    }

    inline bbUINT GetSize() const { return mEntries.GetSize(); }

    inline void* GetObj(int const id)
    {
        return (unsigned)id < GetSize() ? mEntries[id].pObj : NULL;
    }

    int NextID();

    /** Add an object to the registry.
        Double pointers are not checked.
        @param pObj Object pointer
        @return Unique registry ID, or -1 on error
    */
    int Add(void* const pObj);

    /** Delete an object from the registry by its registry ID.
        If the passed ID is -1, the call has no effect.
        @param id Object ID as returned by Add(), can be -1
    */
    inline void Delete(int const id)
    {
        bbASSERT(id < (int)mEntries.GetSize());
        if (id >= 0)
            mEntries[id].pObj = NULL;
    }

    /** Delete an object from the registry.
        Only first hit will be deleted. No effect if object not found.
        @param pObj Object to remove.
    */
    inline void DeleteObj(void* const pObj)
    {
        Delete(Find(pObj));
    }

    /** Find object in registry.
        @param pObj Object to search for
        @return ID if found, or -1 if not found.
    */
    int Find(void* const pObj);

    /** Get number of objects in registry.
        @return Object count
    */
    bbUINT GetCount() const;
};

#endif

