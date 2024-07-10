#ifndef RIB_H
#define RIB_H

#include "bodypart.h"

class Rib : public BodyPart
{
public:
    Rib();
    ~Rib();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;
};

#endif // RIB_H
