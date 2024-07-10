#include "rib.h"

Rib::Rib()
{
    BP_Type = 10;
}

Rib::~Rib()
{
//    boundingBox=nullptr;
////    parentAlienPtr = nullptr;
//    while (!cellList.empty()) delete cellList.last(), cellList.removeLast();

//    vertexList.clear();

//    for (int i=0; i<subBodyParts.length(); i++)
//    {
//        // TODO(ismail): Create a global ENUM of all the bodyparts or a friend class or something
//        // ... cuz any class should be able to access the ENUM
//        // Alternatively use Strings for the bodyparts
//        if (subBodyParts[i]->BP_Type == 0 || subBodyParts[i]->BP_Type == 1) // if its a Joint or a Bone, remove it
//        {
//            BodyPart *tempBP =  subBodyParts[i];
//            subBodyParts.removeAt(i--);
//            delete tempBP;
//        }
//    }

    qDebug() << "Rib removed from Alien";
}

BodyPart *Rib::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    Rib *ribPtr = new Rib();
    this->duplicateAllCommonMembers(ribPtr, parentAlienPtr, tempClonedBP_Checklist);
    return ribPtr;
}
