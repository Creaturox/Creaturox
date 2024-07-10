#ifndef LEVEL_1_2_FEATUREGROWTHQUAD_H
#define LEVEL_1_2_FEATUREGROWTHQUAD_H

#include <QString>

/* Class Description:
 *
 *
*/


class Level_1_2_FeatureGrowthQuad
{
public:
    Level_1_2_FeatureGrowthQuad();

    QString bp_name;
    // measurements for the Quad
    // originating facelist
    // level 1 (muscle) feature name
    // quad (four vertices defining this quad) - vertexList (perhaps a normal vector as well as the last element)
    // level 2 (secondary formations) feature name
    // isSwappable (with another quad)
    // isMovable / isTranslatable (to another location)
};

#endif // LEVEL_1_2_FEATUREGROWTHQUAD_H
