#include "lowerlimb.h"

LowerLimb::LowerLimb()
{
    BP_Type = 2;
}

LowerLimb::~LowerLimb()
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

    qDebug() << "LowerLimb removed from Alien";
}

BodyPart *LowerLimb::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    LowerLimb *llPtr = new LowerLimb();
    this->duplicateAllCommonMembers(llPtr, parentAlienPtr, tempClonedBP_Checklist);
    return llPtr;
}
