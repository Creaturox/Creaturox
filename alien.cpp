#include "alien.h"

Alien::Alien()
{
    BP_ID_Counter = -1;
    clonedFrom = -1;

    seperateCloneBPs.clear();
    BP_ReplacementMap.clear();
    initAlienColorMap();

    alienExtremities.clear();
}

Alien::~Alien()
{
    // All the BPs are deleted in call to AlienModeller before this Alien is deleted

//    this->ruleSetList.clear();
    this->rootBP = nullptr;
    if (this->alienDNA)
        delete this->alienDNA;
    this->cloneBPs.clear();

    qDebug() << "Alien Deleted";
}

void Alien::makeAllRuleSets()
{
    // Rule Set for: Thigh Bone
    RuleSet ruleSetBP_ThighBone;

    ruleSetBP_ThighBone.bodyPartName="THIGHBONE";
    ruleSetBP_ThighBone.maxBranchings = 20;
    ruleSetBP_ThighBone.minBranchings = 8;

//    ruleSetBP_ThighBone.facesToIgnore.append(Box::BoxFace::FRONT);
//    ruleSetBP_ThighBone.facesToIgnore.append(Box::BoxFace::BACK);
//    ruleSetBP_ThighBone.facesToIgnore.append(Box::BoxFace::RIGHT);
//    ruleSetBP_ThighBone.facesToIgnore.append(Box::BoxFace::TOP);
//    ruleSetBP_ThighBone.facesToIgnore.append(Box::BoxFace::LEFT);
//    ruleSetBP_ThighBone.facesToIgnore.append(Box::BoxFace::BOTTOM);


    ruleSetList.append(ruleSetBP_ThighBone);
    rootBP = nullptr;


//    // Rule Set for: Thigh Muscle 1
//    RuleSet rSetBP_ThighBone;

//    rSetBP_ThighBone.maxBranchings = 4;

//    ruleSetList.append(rSetBP_ThighBone);


//    // Rule Set for: UpperLeg (Thigh)
//    RuleSet rSetBP_ThighBone;

//    rSetBP_ThighBone.maxBranchings = 4;

    //    ruleSetList.append(rSetBP_ThighBone);
}

void Alien::getBodyPartListCellListForRendering(QVector<Cell*> &tempCellList)
{
    tempCellList.resize(0);

    for (int i=0; i<bodyParts.length(); i++)
    {
//        if (bodyParts[i]->BP_Type != )
        for (int j=0; j<bodyParts[i]->cellList.length(); j++)
        {
            Cell *tempCellPtr = bodyParts[i]->cellList[j];
            visitCellsRecursively(tempCellPtr, tempCellList);
        }
    }
}

void Alien::visitCellsRecursively(Cell* rootCell, QVector<Cell*> &tempCellList)
{
    tempCellList.append(rootCell);

    // go through all the children of this cell and render them recursively
    for (int i=0; i<rootCell->childCells.length(); i++)
    {
        visitCellsRecursively(rootCell->childCells[i], tempCellList);
    }
}

void Alien::initAlienColorMap()
{
    // setAlienColorMap(2,8, 3,9, 4,10);
    // setAlienColorMap(11,3, 12,1, 13,4);
}

void Alien::setAlienColorMap(int bpType1, int color1, int bpType2, int color2, int bpType3, int color3)
{
    this->BP_TypeColorMap_2.clear();

    // this->BP_TypeColorMap_2.insert(bpType1, color1);
    // this->BP_TypeColorMap_2.insert(bpType2, color2);
    // this->BP_TypeColorMap_2.insert(bpType3, color3);
    // this->BP_TypeColorMap_2.insert(6, 6);
}

float Alien::getZScalingFactorFromBulkinessDNA(BodyPart *bpPtr)
{
    // First check if the entry for that BP is either squashing or stretching
    if (this->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[bpPtr->BP_ID].at(3).x() != -1)
        return this->alienDNA->skeletonDNA->Skeletal_BP_Bulkiness_DNA_ForEachBP[bpPtr->BP_ID].at(3).y();
    else
        return 1.0;
}

void Alien::mapExtremToParentBP(int extremID, int parentBP_ID)
{
    extremToParentBP_Map.insert(extremID, parentBP_ID);
}
