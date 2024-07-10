#include "headsubbp_mandible.h"

HeadSubBP_Mandible::HeadSubBP_Mandible()
{
    BP_Type = 12;
}

HeadSubBP_Mandible::~HeadSubBP_Mandible()
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

        qDebug() << "HeadSubBP_Mandible removed from Alien";
}

BodyPart *HeadSubBP_Mandible::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    HeadSubBP_Mandible *mandiblePtr = new HeadSubBP_Mandible();
    this->duplicateAllCommonMembers(mandiblePtr, parentAlienPtr, tempClonedBP_Checklist);
    return mandiblePtr;
}
