#ifndef EXTREMITYBP_DIGIT_H
#define EXTREMITYBP_DIGIT_H

#include "bodypart.h"

// In addition to simple linear curves (using JAT), this BP can also take a more twisty/curly/winding shape
// Potentailly it can also have branched out lineages, instead of only a single lineage
class ExtremityBP_Digit : public BodyPart
{
public:
    ExtremityBP_Digit();
    ~ExtremityBP_Digit();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;

};

#endif // EXTREMITYBP_DIGIT_H
