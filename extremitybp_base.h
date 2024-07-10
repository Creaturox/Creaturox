#ifndef EXTREMITYBP_BASE_H
#define EXTREMITYBP_BASE_H

#include "extremitybp_base_carpal.h"

// This class houses a set of closely packed lineages that form the base of an extremity
// such as a Hand, Feet and paws.
class ExtremityBP_Base
{
public:
    ExtremityBP_Base();
    ~ExtremityBP_Base();

    void getAllCarpals(QVector<BodyPart*> &carpals);

    // A set of lineages with varied shapes packed/stacked together to form the extremity base
    // EAch of these follow a DNA spec
    int BP_Type;
    int BP_ID;
    QVector<BodyPart*> extremBP_BaseCarpals;
    QVector<QPair<qfloat16,QVector3D>> extDigitAttachmentAnglesAndPnts;
};

#endif // EXTREMITYBP_BASE_H
