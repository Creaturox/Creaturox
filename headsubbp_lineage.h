#ifndef HEADSUBBP_LINEAGE_H
#define HEADSUBBP_LINEAGE_H

#include "bodypart.h"

// Each Head SubBP is composed of 1 or more lineages, which is in turn a BodyPart

class HeadSubBP_Lineage : public BodyPart
{
public:
    HeadSubBP_Lineage();
    ~HeadSubBP_Lineage();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;
};

#endif // HEADSUBBP_LINEAGE_H
