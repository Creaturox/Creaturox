#ifndef RULESET_H
#define RULESET_H

#include <qvector.h>


class RuleSet
{
public:
    RuleSet();

    QString bodyPartName;
    int minBranchings;
    int maxBranchings;
    bool makeBranchingsInZones;
    int minBranchingsInZones;
    int maxBranchingsInZones;
    int maxBranchingAngle; // angle from a face vector
    int minBranchingAngle;
    int maxLevels;
    int minLevels;
    int maxZonesToCreate;
    int minZonesToCreate;
    bool mutateInZones;
    QVector<float> maxScalingsXYZ; // 3 entries. one each for x, y and z
    QVector<float> minScalingsXYZ; // 3 entries. one each for x, y and z
    QVector<int> cellsToIgnore;
    QVector<int> facesToIgnore;
    QVector<float> pointsToIgnore;
    QVector<int> zonesToIgnore;

    // other BPs NOT to attach
    // min number of BPs to attach to
    // max number of BPs to attach to
};

#endif // RULESET_H

/*
 *
 *****  RuleSet *******************
 *
 *      Definition: a set of rules to be applied on the body part formation algorithms, e.g. which faces to ignore for branching etc.
 *      The RuleSet will probably grow to be much bigger for the entire alien modelling system
 *
 *      Members:
 *          Max branchings
 *          (Max/Min) Angle of branching
 *          Cells to ignore
 *          faces to ignore
 *          points to ignore
 *          zones to ignore
 *          maximum levels to go for => as in stairs
 *          Maximum zones to create
 *          Max amount of cell scaling
 *          Min amount of cell scaling
 *
 *      What is a Zone?? A zone can be an area on the bp
 *
*/
