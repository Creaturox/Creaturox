#ifndef LOWERLIMB_H
#define LOWERLIMB_H

#include "bodypart.h"


class LowerLimb : public BodyPart
{
public:
    LowerLimb();
    ~LowerLimb();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;
};

#endif // LOWERLIMB_H
