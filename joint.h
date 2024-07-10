#ifndef JOINT_H
#define JOINT_H

#include "bodypart.h"


//  A joint is a small BP that connects with other level 0, 1 and 2 BPs. Most of the time it attaches
//  to bones or cartiledges.
//  Representation: A joint is represented by a box (at the moment. later will add new and interesting geometry to it...Johnson's polyhedra)


class Joint : public BodyPart
{
public:
    Joint();
    ~Joint();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;

    QVector<int> allAttachedBP_IDs; // all the BPs attached to this joint

    int numSubCells; // [TODO(ismail): Perhaps not needed, so remove it] randomly selected from the jointSpecsBrackets in Joint_RuleSet
                     // purpose of sub cells: some sub cells will be used as origin points of
                     // bones(level 0 BP), while others will be used to generate and grow level 1 or 2 BPs on
                     // the joints
};

#endif // JOINT_H
