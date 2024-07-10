#include "headsubbp_zygomatic.h"

HeadSubBP_Zygomatic::HeadSubBP_Zygomatic()
{
    BP_Type = 13;
}

HeadSubBP_Zygomatic::~HeadSubBP_Zygomatic()
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

    qDebug() << "Zygomatic removed from Alien";
}

BodyPart *HeadSubBP_Zygomatic::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    HeadSubBP_Zygomatic *zygPtr = new HeadSubBP_Zygomatic();
    this->duplicateAllCommonMembers(zygPtr, parentAlienPtr, tempClonedBP_Checklist);
    return zygPtr;
}
