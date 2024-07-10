#ifndef LEVEL_1_2_FEATUREPLACEHOLDER_H
#define LEVEL_1_2_FEATUREPLACEHOLDER_H

#include "quadface.h"

#include <QString>

/* Class Description:
 * A place holder for level 1 and 2 features. A placeholder will be represented as a Quad
 * Level 1 features: Base muscles...perhaps cartileges
 * Level 2 features: Secondary growth formations/mutations
*/


class Level_1_2_FeaturePlaceHolder
{
public:
    Level_1_2_FeaturePlaceHolder();

    int placeholderID;
    QString originatingBP; // bp originates from. Change it to the skeleton of Level 0 BP
    int originatingFaceNum;
    int featureLevel;
    int featureID;
    QuadFace *placeholderQuad; //four vertices and a normal defining this placeholder
    bool isSwappable;
    bool isMovable;
    // bool isTiltable
};

#endif // LEVEL_1_2_FEATUREPLACEHOLDER_H
