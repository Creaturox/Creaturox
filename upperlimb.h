#ifndef UPPERLIMB_H
#define UPPERLIMB_H

#include "bodypart.h"


class UpperLimb : public BodyPart
{
public:
    UpperLimb();
    ~UpperLimb();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;
};

#endif // UPPERLIMB_H
