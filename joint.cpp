#include "joint.h"

Joint::Joint()
{
    BP_Type = 0;

    allAttachedBP_IDs.clear();
}

Joint::~Joint()
{
    allAttachedBP_IDs.clear();
//    qDebug() << "Joint removed from Alien";
}

BodyPart *Joint::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    qDebug() << "joint duplicateBP() method called";

    Joint *jPtr = new Joint();
    this->duplicateAllCommonMembers(jPtr, parentAlienPtr, tempClonedBP_Checklist);
    return jPtr;
}

