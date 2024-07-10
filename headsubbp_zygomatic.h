#ifndef HEADSUBBP_ZYGOMATIC_H
#define HEADSUBBP_ZYGOMATIC_H

#include "bodypart.h"



class HeadSubBP_Zygomatic : public BodyPart
{
public:
    HeadSubBP_Zygomatic();
    ~HeadSubBP_Zygomatic();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;
};

#endif // HEADSUBBP_ZYGOMATIC_H
