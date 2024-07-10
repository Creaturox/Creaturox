#include "ridge.h"

Ridge::Ridge()
{
    ridgeLinesList.resize(0);
}

Ridge::~Ridge()
{

}

BodyPart *Ridge::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    qDebug() << "Head duplicateBP() method called";

    Ridge *rPtr = new Ridge();
    this->duplicateAllCommonMembers(rPtr, parentAlienPtr, tempClonedBP_Checklist);
    return rPtr;
}
