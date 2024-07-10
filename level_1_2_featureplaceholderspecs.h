#ifndef LEVEL_1_2_FEATUREPLACEHOLDERSPECS_H
#define LEVEL_1_2_FEATUREPLACEHOLDERSPECS_H

#include <qvector.h>
#include <qvector3d.h>


class Level_1_2_FeaturePlaceholderSpecs
{
public:
    Level_1_2_FeaturePlaceholderSpecs();

    QString originLevel_0_BP_ID;
    float originLevel_0_BP_SelectionStrength; // percentage of selection. (probability of selection)

    QVector<QString> BP_Candidates; // 3 entries priority wise (featureID refers to the candidate feature to be added to this placeholder. features are read
                            // from OBJ file since they are modelled in Maya)
    QVector<float> BP_SelectionStrength;

    QVector<int> BP_LevelCandidates; // 2 entries priority wise.
    QVector<float> BP_LevelSelectionStrength;

    QVector<int> faceCandidates; // 3 faces sorted with priority. Consider using ENUMerations
    QVector<float> faceSelectionStrength; // 3 percentages for each of the entry in faceNum

    QVector<int> startEdge; // these three edges correspond to the candidate feature that is to be attached to this placeholder.
                                           // Each edge is represented as 2 points

    QVector<float> percentDistFromStartEdge;

    // edge offset percentage ==> to tell the system how much offset the placeholder can be from the starting edge

//    QVector<bool> isSwappable;
//    QVector<float> isSwappablePercentages;

//    QVector<bool> isMovable;
//    QVector<float> isMovablePercentages;

     bool isTiltable; // some placeholders can be angled/tilted/rotated so they can either be twisted or intersect two faces
//     bool isScalable;
};

#endif // LEVEL_1_2_FEATUREPLACEHOLDERSPECS_H

/*
 *
*/
