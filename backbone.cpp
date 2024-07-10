#include "backbone.h"

BackBone::BackBone()
{
    BP_Type = 4;
}

BackBone::~BackBone()
{
//    boundingBox=nullptr;
//    parentAlienPtr = nullptr;
//    cellList.clear();
//    vertexList.clear();

//    for (int i=0; i<subBodyParts.length(); i++)
//    {
//        if (subBodyParts[i]->BP_Type == 0 || subBodyParts[i]->BP_Type == 1)
//        {
//            BodyPart *tempBP =  subBodyParts[i];
//            subBodyParts.removeAt(i--);
////            subBodyParts[i] = nullptr;
//            delete tempBP;
//        }
//    }


    qDebug() << "BackBone removed from Alien";
}

BodyPart *BackBone::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    qDebug() << "BackBone duplicateBP() method called";

    BackBone *bbPtr = new BackBone();
    this->duplicateAllCommonMembers(bbPtr, parentAlienPtr, tempClonedBP_Checklist);
    return bbPtr;
}

