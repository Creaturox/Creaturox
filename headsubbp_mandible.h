#ifndef HEADSUBBP_MANDIBLE_H
#define HEADSUBBP_MANDIBLE_H

#include "bodypart.h"


class HeadSubBP_Mandible : public BodyPart
{
public:
    HeadSubBP_Mandible();
    ~HeadSubBP_Mandible();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist);
};

#endif // HEADSUBBP_MANDIBLE_H
