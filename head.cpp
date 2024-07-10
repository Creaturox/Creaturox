#include "head.h"

Head::Head()
{
    subHeadJnt2JntLineages.clear();
    BP_Type = 6;
}

Head::~Head()
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

    // TODO(ismail): Also delete all the entries in allCellListLineages


        qDebug() << "Head removed from Alien";
}

BodyPart *Head::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    Head *hPtr = new Head();
    this->duplicateAllCommonMembers(hPtr, parentAlienPtr, tempClonedBP_Checklist);
    return hPtr;
}
