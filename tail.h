#ifndef TAIL_H
#define TAIL_H

#include "bodypart.h"



class Tail : public BodyPart
{
public:
    Tail();
    ~Tail();

    BodyPart *duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist) override;
};

#endif // TAIL_H
