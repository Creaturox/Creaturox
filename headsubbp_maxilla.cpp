#include "headsubbp_maxilla.h"

HeadSubBP_Maxilla::HeadSubBP_Maxilla()
{
    BP_Type = 11;
}

HeadSubBP_Maxilla::~HeadSubBP_Maxilla()
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

    qDebug() << "Maxilla removed from Alien";
}

BodyPart *HeadSubBP_Maxilla::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    HeadSubBP_Maxilla *maxPtr = new HeadSubBP_Maxilla();
    this->duplicateAllCommonMembers(maxPtr, parentAlienPtr, tempClonedBP_Checklist);
    return maxPtr;
}

