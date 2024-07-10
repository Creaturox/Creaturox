#include "bone.h"

Bone::Bone()
{
    BP_Type = 1;

    endJoints.first = -1;
    endJoints.second = -1;

    allAttachedBP_IDs.clear();

    boneRidgeSpecs_global.resize(0);
    boneRidgeSpecs_local.clear();

    rootCell = nullptr;
    allStackedDenserLineages.clear();
    subdivisionCells.clear();
    nonRenderableCells.clear();
}

Bone::~Bone()
{
    // allStackedDenserLineages.clear();
    allAttachedBP_IDs.clear();
    allStackedDenserLineages.clear();
    subdivisionCells.clear();
    nonRenderableCells.clear();
    // while (!allStackedDenserLineages.empty()) delete allStackedDenserLineages.last(), allStackedDenserLineages.removeLast();
    // while (!subdivisionCells.empty()) delete subdivisionCells.last(), subdivisionCells.removeLast();
    // while (!nonRenderableCells.empty()) delete nonRenderableCells.last(), nonRenderableCells.removeLast();
    rootCell = nullptr;

    // subdivisionCells.clear();
    // nonRenderableCells.clear();
}

BodyPart *Bone::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    Bone *bPtr = new Bone();
    bPtr->endJoints.first = this->endJoints.first;
    bPtr->endJoints.second = this->endJoints.second;

    // TODO(ismail): copy ridges data using seperate methods

    this->duplicateAllCommonMembers(bPtr, parentAlienPtr, tempClonedBP_Checklist);
    return bPtr;
}

QVector<Cell*> Bone::getAllLeafDensers()
{
//    QVector<Cell*> leafDensers; leafDensers.clear();
//    for (int i=0; i<allStackedDenserLineages.length(); i++)
//    {
//        leafDensers.append(allStackedDenserLineages.at(i).last());
//    }
//    return leafDensers;
}

Cell *Bone::getLeafDenser()
{
    if (!allStackedDenserLineages.isEmpty())
        return allStackedDenserLineages.last();
    return nullptr;
}

void Bone::bevelBone()
{
    rootCell->bevelCellBox(20);
}

//bool Bone::deleteCells(QVector<Cell *> inputCells)
//{
//    QVector<Cell*> cellsToDelete; cellsToDelete.clear();
//    for (int k=1; k<inputCells.length(); k++)
//    {
//        Cell *tempCellPtr = inputCells[k];
//        visitCellsRecursively(tempCellPtr, cellsToDelete);
//    }

//    if (!cellsToDelete.empty())
//    {
//        while (!cellsToDelete.empty()) delete cellsToDelete.last(), cellsToDelete.removeLast();
////        while (cellList.length() > 1)
////            cellList.removeLast();

//        qDebug() << "all cells deleted except for the first one";
//        return true;
//    }
//    else
//    {
//        qDebug() << "no cells to be deleted. This bone might only has one cell i.e. no densifiers";
//        return false;
//    }

//}
