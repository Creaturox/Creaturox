#include "headsubbp_lineage.h"

HeadSubBP_Lineage::HeadSubBP_Lineage()
{

}

HeadSubBP_Lineage::~HeadSubBP_Lineage()
{

}


BodyPart *HeadSubBP_Lineage::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    HeadSubBP_Lineage *headSubBPLinPtr = new HeadSubBP_Lineage();
    this->duplicateAllCommonMembers(headSubBPLinPtr, parentAlienPtr, tempClonedBP_Checklist);
    return headSubBPLinPtr;
}
