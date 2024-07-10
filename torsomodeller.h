#ifndef TORSOMODELLER_H
#define TORSOMODELLER_H

#include <uppertorso.h>



/*
 * This class is responsible for procedurally modelling the upper and lower torsos
 * of the alien creature.
*/

class TorsoModeller
{
public:
    TorsoModeller();

    UpperTorso* modelUpTorso();
};

#endif // TORSOMODELLER_H
