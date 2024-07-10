#include "upperlimb.h"

UpperLimb::UpperLimb()
{
    BP_Type = 3;
}

UpperLimb::~UpperLimb()
{
//    parentAlienPtr = nullptr;
//    for (int i=0; i<cellList.length(); i++)
//        delete cellList[i];
//    cellList.clear();

//    for (int i=0; i<subBodyParts.length(); i++)
//    {
//        if (subBodyParts[i]->BP_Type == 0 || subBodyParts[i]->BP_Type == 1)
//        {
//            BodyPart *tempBP =  subBodyParts[i];
//            subBodyParts.removeAt(i--);
//            delete tempBP;
//        }
//    }

//    vertexList.clear();
//    grownBodyParts.clear();

    qDebug() << "UpperLimb removed from Alien";
}

BodyPart *UpperLimb::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    UpperLimb *ulPtr = new UpperLimb();
    this->duplicateAllCommonMembers(ulPtr, parentAlienPtr, tempClonedBP_Checklist);
    return ulPtr;
}
