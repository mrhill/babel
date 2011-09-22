#include "mem.h"
#include "str.h"
#include "Tree.h"

bbTree::bbTree(bbUINT const datasize, bbUINT const leveloffs,  bbUINT const indexenlarge)
  : mNodeSize((bbU8)(datasize + sizeof(bbTreeNode))),
    mLevelOffs((bbU8)(leveloffs + sizeof(bbTreeNode))),
    mNodeIdxEnlarge((bbU8)indexenlarge)
{
    bbASSERT((datasize | leveloffs | indexenlarge) < (256-sizeof(bbTreeNode)));

    mpNodes = NULL;
    mTreeSize = 0;
    mNodeRoot = (bbU32)-1;
    mNodeFree = 0;
}

bbTree::~bbTree()
{
    bbMemFree(mpNodes);
}

void bbTree::Clear()
{
    bbMemFreeNull((void**)&mpNodes);
    mTreeSize = 0;
    mNodeRoot = (bbU32)-1;
    mNodeFree = 0;
}

bbU32 bbTree::NewNode()
{
    bbU32 i;
    bbUINT const nodesize = mNodeSize;

    if (mNodeFree >= mTreeSize)
    {
        bbASSERT(mNodeFree == mTreeSize);

        const bbUINT newsize = mTreeSize + mNodeIdxEnlarge;
        if (bbEOK != bbMemRealloc(newsize * nodesize, &mpNodes))
            return (bbU32)-1;
        mTreeSize = newsize;

        i = mNodeFree;
        bbU32 const i_end = mTreeSize;
        bbTreeNode* pNode = GetNode(i);
        do
        {
            pNode->mLT = ++i;
            pNode = (bbTreeNode*)((bbU8*)pNode + nodesize);

        } while (i < i_end);
    }

    i = mNodeFree;
    mNodeFree = GetNode(i)->mLT;
    return i;
}

