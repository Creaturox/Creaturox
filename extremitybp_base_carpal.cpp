#include "extremitybp_base_carpal.h"

ExtremityBP_Base_Carpal::ExtremityBP_Base_Carpal()
{
    BP_Type = 17;
}

ExtremityBP_Base_Carpal::~ExtremityBP_Base_Carpal()
{
    // qDebug() << "ExtremityBP_Base_Carpal deleted";
}

BodyPart *ExtremityBP_Base_Carpal::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{

}
