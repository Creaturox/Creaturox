#ifndef EXTREMITYBP_BASE_CARPAL_H
#define EXTREMITYBP_BASE_CARPAL_H

#include "bodypart.h"

class ExtremityBP_Base_Carpal : public BodyPart
{
public:
    ExtremityBP_Base_Carpal();
    ~ExtremityBP_Base_Carpal();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;
};

#endif // EXTREMITYBP_BASE_CARPAL_H
