#include "tail.h"

Tail::Tail()
{
    BP_Type = 5;
}

Tail::~Tail()
{

}

BodyPart *Tail::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    Tail *tPtr = new Tail();
    this->duplicateAllCommonMembers(tPtr, parentAlienPtr, tempClonedBP_Checklist);
    return tPtr;
}
