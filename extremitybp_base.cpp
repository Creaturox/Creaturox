#include "extremitybp_base.h"

ExtremityBP_Base::ExtremityBP_Base()
{
    extremBP_BaseCarpals.clear();
    extDigitAttachmentAnglesAndPnts.clear();
}

ExtremityBP_Base::~ExtremityBP_Base()
{
    extremBP_BaseCarpals.clear();
    extDigitAttachmentAnglesAndPnts.clear();
    // qDebug() << "ExtremityBP_Base deleted";
}

void ExtremityBP_Base::getAllCarpals(QVector<BodyPart *> &carpals)
{
    for (BodyPart* carpal : extremBP_BaseCarpals)
        carpals.append(carpal);
}


