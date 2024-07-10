#include "extremitybp.h"

ExtremityBP::ExtremityBP()
{
    extremBP_basePtr = nullptr;
    extremBP_Digits.clear();
}

ExtremityBP::~ExtremityBP()
{
    delete extremBP_basePtr;
    extremBP_basePtr = nullptr;
    extremBP_Digits.clear();
    qDebug() << "extremity BP deleted";
}

void ExtremityBP::getAllCarpalsAndDigits(QVector<BodyPart*> &digits, QVector<BodyPart*> &carpals)
{
    for (BodyPart* dig : extremBP_Digits)
        digits.append(dig);

    extremBP_basePtr->getAllCarpals(carpals);
}

bool ExtremityBP::detachSubCarpalsFromRootCarpal()
{
    bool anyCarpalsDetached=false;
    BodyPart *rootCarpalPtr = extremBP_basePtr->extremBP_BaseCarpals.first();
    for (BodyPart *subCarpal : rootCarpalPtr->subBodyParts)
    {
        if (extremBP_basePtr->extremBP_BaseCarpals.contains(subCarpal))
        {
            rootCarpalPtr->subBodyParts.removeOne(subCarpal);
            anyCarpalsDetached=true;
        }
    }
    return anyCarpalsDetached;
}

bool ExtremityBP::attachSubCarpalsToRootCarpal()
{
    detachSubCarpalsFromRootCarpal();
    bool anyCarpalsAttached=false;
    BodyPart *rootCarpalPtr = extremBP_basePtr->extremBP_BaseCarpals.first();
    for (int i=1; i<extremBP_basePtr->extremBP_BaseCarpals.length(); i++)
    {
        if (!rootCarpalPtr->subBodyParts.contains(extremBP_basePtr->extremBP_BaseCarpals[i]))
        {
            rootCarpalPtr->subBodyParts.append(extremBP_basePtr->extremBP_BaseCarpals[i]);
            anyCarpalsAttached=true;
        }
    }
    return anyCarpalsAttached;
}
