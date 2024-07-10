#include "skeleton.h"

Skeleton::Skeleton()
{
    // rootBP_Ptr = nullptr;
}

Skeleton::~Skeleton()
{
    qDebug() << "Deleting Skeleton";
    QMapIterator<int, QPolygon*> iter(Jnt2JntRelations_TypeGuided);
    while (iter.hasNext())
    {
        iter.next();
        iter.value()->clear();
    }
    this->Jnt2JntRelations_TypeGuided.clear();
    vectorLocationCoupleForEachJoint_TypeGuided.clear();
    JntToJntMappings_ForBookKeeping.clear();
    skeleton_specs.clear();
    Jnt2JntRelations.clear();
    qDebug() << "Skeleton Deleted";
}

// void Skeleton::setRootBP(BodyPart *bpPtr)
// {
//     rootBP_Ptr = bpPtr;
// }

// BodyPart *Skeleton::getRootBP()
// {
//     return rootBP_Ptr;
// }
