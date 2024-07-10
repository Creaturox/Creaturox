#ifndef EXTREMITYBP_H
#define EXTREMITYBP_H

#include "extremitybp_base.h"
#include "extremitybp_digit.h"

// This class can act as Hands, Feet, Paws etc
// This is the enclosing Extremity BP that is composed of just one ExtremityBP_Base and one or many ExtremityBP_Digits
class ExtremityBP
{
public:
    // Define copy semantics
    ExtremityBP();
    ~ExtremityBP();

    void getAllCarpalsAndDigits(QVector<BodyPart*> &digits, QVector<BodyPart*> &carpals);
    bool detachSubCarpalsFromRootCarpal();
    bool attachSubCarpalsToRootCarpal();

    int BP_Type;
    int BP_ID;
    ExtremityBP_Base* extremBP_basePtr;
    QVector<BodyPart*> extremBP_Digits;
};

#endif // EXTREMITYBP_H
