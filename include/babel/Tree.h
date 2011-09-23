#ifndef bbTree_H_
#define bbTree_H_

struct bbTreeNode
{
    bbU32 mLT;      //!< index of node with offset less than this node, (bbU32)-1 is NIL
    bbU32 mGE;      //!< index of node with offset greater or equal than this node, (bbU32)-1 is NIL
    // Node payload follows here
};

/** Base for binary tree implementations. */
class bbTree
{
protected:
    void*       mpNodes;        //!< Tree nodes, linear array
    bbU32       mTreeSize;      //!< Capacity of mpNodes[]
    bbU8 const  mNodeSize;
    bbU8 const  mLevelOffs;
    bbU8 const  mNodeIdxEnlarge;
    bbU32       mNodeRoot;      //!< Index of tree root, -1 if empty
    bbU32       mNodeFree;      //!< Index of first free entry, 0 if empty

    /** Get node by index.
        This is a slow implementation, because it uses a multiplication.
        bbTree derivations should provide their own GetNode() override.
    */
    inline bbTreeNode* GetNode(bbU32 const idx) const
    {
        return (bbTreeNode*)((bbU8*)mpNodes + idx * mNodeSize);
    }

    bbU32 LinkNode(bbU32 idx);

    /** Allocate a new node.
        The allocated node struct will be unitialized.
        This function invalidates any node pointers, because it may reallocate the mpNodes[] array.
        @return Index into mpNodes[], or (bbU32)-1 on error
    */
    bbU32 NewNode();

public:
    bbTree(bbUINT const datasize, bbUINT const leveloffs, bbUINT const indexenlarge = 32);
    ~bbTree();
    void Clear();

};

#endif /* bbTree_H_ */

