#ifndef UPPERTORSO_H
#define UPPERTORSO_H

#include <box.h>

/*
 * This class arranges all the boxes that make up the torso in a tree structure
*/

class UpperTorso
{
public:
    UpperTorso();

    Box *upTorsoBoxTree; // pointer to the root node i.e. the Box on the top of the tree

// some methods to add new nodes (boxes) to the tree
//    void addChildBox(Box *childBox);

// some methods for searching in the tree and also traversal in the tree such as inorder, postorder or preorder traversals
};

#endif // UPPERTORSO_H
