#include "extremitybp_digit.h"

ExtremityBP_Digit::ExtremityBP_Digit()
{
    BP_Type = 18;
}

ExtremityBP_Digit::~ExtremityBP_Digit()
{
    // qDebug() << "ExtremityBP_Digit deleted";
}

BodyPart *ExtremityBP_Digit::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{

}
