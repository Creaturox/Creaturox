#include "ruleset.h"

RuleSet::RuleSet()
{
    // Initialize to default values
    bodyPartName="NONE";

    minBranchings=0;
    maxBranchings=0;
    makeBranchingsInZones=false;
    maxBranchingAngle=10;
    minBranchingAngle=30;
    maxLevels=0;
    minLevels=0;
    maxZonesToCreate=0;
    minZonesToCreate=0;
    minScalingsXYZ.resize(3);
    minScalingsXYZ[0]=0; minScalingsXYZ[1]=0; minScalingsXYZ[2]=0;
    maxScalingsXYZ.resize(3);
    maxScalingsXYZ[0]=0; maxScalingsXYZ[1]=0; maxScalingsXYZ[2]=0;
    cellsToIgnore.resize(0);
    facesToIgnore.resize(0);
    pointsToIgnore.resize(0);
    zonesToIgnore.resize(0);
}
