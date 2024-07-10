#include "bodypart.h"

BodyPart::BodyPart()
{
    BP_TranslateMatrix.setToIdentity();
    // TODO(ismail): initialize other things as well
    // initialize the celllist, subbodyparts, vertexlist, grownBPs
    // initialize pivot vector
    // clonedFrom
    // level
    // bounding box
    // bodypart type ID
    // Orientation Quat
    // Tranlation Matrix
    BP_ID = -1;
    rootJntID = -1;
    BP_OrientationQuat = QQuaternion::fromDirection(QVector3D(0,0,1), QVector3D(0,0,1));
    clonedFrom = -1;
    cloneFromAlienAndBP_Pair.first=-1; cloneFromAlienAndBP_Pair.second=-1;
    BP_PivotPt = QVector3D(0,0,0);
    BP_PivotVector = QVector3D(0,0,1.0);
    smoothnessLevel = 0;
    isSymmetric = false;
    mirrorOf = -1;
    boundaryDnsrs.clear();
}

// This desctructor only deletes the joints and bones of BPs. The main BP
// is removed from the AlienModeller's removeAlienBodyParts() method
BodyPart::~BodyPart()
{
    boundingBox=nullptr;
//    parentAlienPtr = nullptr;
    while (!cellList.empty()) delete cellList.last(), cellList.removeLast();

    // Now here we will delete all the Joints and Bones of this BP
    for (int i=0; i<subBodyParts.length(); i++)
    {
        // TODO(ismail): Create a global ENUM of all the bodyparts or a friend class or something
        // ... cuz any class should be able to access the ENUM
        // Alternatively use Strings for the bodyparts
        if (subBodyParts[i]->BP_Type == 0 || subBodyParts[i]->BP_Type == 1) // if its a Joint or a Bone, remove it
        {
            BodyPart *jointOrBoneToDelete =  subBodyParts[i];
            subBodyParts.removeAt(i--);
            delete jointOrBoneToDelete;
        }
    }
    subBodyParts.clear();
    subExtremityBPsIDs.clear();

    qDebug() << "BP removed from Alien";
}

void BodyPart::makeCell(QVector3D cellLocation, GLfloat cellEnergy)
{
    // TODO(ismail): Embed this inside a method that can return a pointer to a Cell instance
    //               Also this should call a method on cell that will accept the location and energy and make a box (similar to makeBox() method of Cell)
    Cell *tempCell = new Cell();
    tempCell->cellEnergy=cellEnergy;
    tempCell->cellCenterLocation=cellLocation;
    tempCell->makeBox();
    cellList.append(tempCell);

    // whenever a new cell is added, update the pivot point of the entire BP (Joint/Bone/Ridge)
//    BP_PivotPt = cellLocation;

    // Update the tranlation matrix of this BP. This will be useful when transformations are performed on
    BP_TranslateMatrix.translate(cellList[0]->getPivotPoint());
}

QVector3D BodyPart::getPivotPoint()
{
    // Get the pivot point of the first cell of the root joint
    // *** For Head the pivot point will be the joint somewhere in the middle of the lineage
    // QVector<int> jntLineageList_IDs; jntLineageList_IDs.clear();
    // if (this->BP_Type == 6) // If this is a HEAD - usually this is needed for attaching the Head to another BP
    // {
    //     // Get an ordered list of joints and select the mid joint as the pivot joint
    //     for (BodyPart *jnt : this->subBodyParts)
    //     {
    //         if (jnt->BP_Type == 0)
    //             jntLineageList_IDs << jnt->BP_ID;
    //     }
    //     rootJntID = jntLineageList_IDs[static_cast<int>(floor(jntLineageList_IDs.length()/2.f))];
    // }
    BP_PivotPt = getSubBP_Ptr(rootJntID)->cellList[0]->getPivotPoint();
    // jntLineageList_IDs.clear();
    return BP_PivotPt;
}

QVector3D BodyPart::getPivotVector()
{
    // QVector<int> jntLineageList_IDs; jntLineageList_IDs.clear();
    // if (this->BP_Type == 6) // If this is a HEAD - usually this is needed for attaching the Head to another BP
    // {
    //     // Get an ordered list of joints and select the mid joint as the pivot joint
    //     for (BodyPart *jnt : this->subBodyParts)
    //     {
    //         if (jnt->BP_Type == 0)
    //             jntLineageList_IDs << jnt->BP_ID;
    //     }
    //     rootJntID = jntLineageList_IDs[static_cast<int>(floor(jntLineageList_IDs.length()/2.f))];
    // }
    BP_PivotVector = getSubBP_Ptr(rootJntID)->cellList[0]->getPivotVector();
    // jntLineageList_IDs.clear();
    return BP_PivotVector;
}

BodyPart *BodyPart::getSubBP_Ptr(int BP_ID)
{
    for (int i=0; i<subBodyParts.length(); i++)
    {
        if (BP_ID == subBodyParts[i]->BP_ID)
        {
            return subBodyParts[i];
        }
    }
    return this;
}

void BodyPart::duplicateAllCommonMembers(BodyPart *clonedBP, Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
/*
    int BP_Type; // an integer value specifying the type of the BP. All the different variations of a particular BP will
                 // ... share the same BP_Type
    int BP_ID;
    Box *boundingBox; // a bounding volume algorithm would be good such as a convex hull
    int level; // perhaps not needed
    int clonedFrom; // the BodyPart it was cloned from
//    int cellCount;
    QVector<Cell> cellList; // list of sub cells. If a body part is made out of several cells, the first cell should be the
                            // ... be the leader cell. This kind of topology is important for a variety of
                            // ... alien muscles and tissues
    QVector<BodyPart*> subBodyParts; // list of sub body part pointers. These could be joints and bones and other BPs etc
    QVector<QVector3D> vertexList; // TODO(ismail): Perhaps need to make it a list of vertex indices for transformation purposes
    // TODO (ismail): Perhaps the below quaternions/matrices are to be used for the Bounding Box/Volume for this BP
    QQuaternion BP_OrientationQuat; // used to keep a track of the rotations
    QMatrix4x4 BP_TranslateMatrix; // used to keep a track of the translations
    QVector3D BP_PivotPt; // A 3D point that serves as the pivot point for rotations, scalings and translations
                          // A pivot point can also be used as the location/originating/central point
    // a list of all faces of this BP (to be used for placeholders) - for advanced algorithms
    // parentBodyPartPointer
    int rootJntID;
    QVector3D BP_PivotVector;   // A 'Pivot Vector' is a vector that is the orientation vector of a box. It is computed as:
                                // ... center point of Front Face of root joint minus center point of back Face of root joint
    QVector<BodyPart*> grownBodyParts;  //  other BPs grown from this BP in a later stage. These can be skeletal,
                                        //  ... as well as muscular
*/
    clonedBP->parentAlienPtr = parentAlienPtr; // PROBLEM!! try not to have an alien pointer inside BodyPart...Inspect this
    clonedBP->BP_Type = this->BP_Type;
    clonedBP->BP_ID = this->BP_ID; // PROBLEM!!
    clonedBP->clonedFrom = this->BP_ID;     // specify which exact BP it is cloned from
    clonedBP->cloneFromAlienAndBP_Pair = this->cloneFromAlienAndBP_Pair;

    // TODO(ismail): This step might not be needed as vertexList is not being used elsewhere
//    for (int i=0; i<this->vertexList.length(); i++)
//    {
//        clonedBP->vertexList.append(QVector3D(this->vertexList[i].x(),this->vertexList[i].y(),this->vertexList[i].z()));
//    }

    clonedBP->BP_OrientationQuat = this->BP_OrientationQuat;
    clonedBP->BP_TranslateMatrix = this->BP_TranslateMatrix;
    // TODO(ismail): Do we need a scaling matrix/factor cloned here as well?
    clonedBP->BP_PivotPt = this->getPivotPoint();
    clonedBP->rootJntID = this->rootJntID; // PROBLEM!! because the jnt IDs will differ amongst clones
    clonedBP->BP_PivotVector = this->getPivotVector();

    // duplicate cell list
    // TODO(ismail): Perhaps call makeCell(), so we won't need to duplicate the cells and their boxes
    for (int i=0; i<this->cellList.length(); i++)
    {
        Cell *tempCellPtr = this->cellList[i];
        // create a new cell
        Cell *tempNewCellPtr = new Cell();
        tempCellPtr->cloneCell(tempNewCellPtr);
        clonedBP->cellList.append(tempNewCellPtr);
        // recursive method to copy the cell properties
    }

    // duplicate its sub body parts. This will create clones of joints, bones, and any other sub BPs that are attached to incoming BP
    for (int i=0; i<this->subBodyParts.length(); i++)
    {
        BodyPart *newSubBP_Ptr = this->subBodyParts[i]->duplicateBP(parentAlienPtr, tempClonedBP_Checklist);
        clonedBP->subBodyParts.append(newSubBP_Ptr);
    }

    // set the flag for this BP that it has been cloned in clonedBP_Checklist, which is passed to this
    // ... method
    tempClonedBP_Checklist.insert(this->BP_ID,true);
}

void BodyPart::getBodyPartCellList(QVector<Cell *> &tempCellList)
{
//        tempCellList.resize(0);
    for (int k=0; k<this->cellList.length(); k++)
    {
        Cell *tempCellPtr = this->cellList[k];
        visitCellsRecursively(tempCellPtr, tempCellList);
    }

    for (int j=0; j<this->subBodyParts.length(); j++)
    {
        this->subBodyParts[j]->getBodyPartCellList(tempCellList);
    }
}

void BodyPart::visitCellsRecursively(Cell *rootCell, QVector<Cell *> &tempCellList)
{
    if (!tempCellList.contains(rootCell))
        tempCellList.append(rootCell);

    // go through all the children of this cell and render them recursively
    for (int i=0; i<rootCell->childCells.length(); i++)
    {
        visitCellsRecursively(rootCell->childCells[i], tempCellList);
    }
}

void BodyPart::updateTranslationMatrixForAllSubBPs(QMatrix4x4 BP_TranslateMatrix)
{
    for (int j=0; j<this->subBodyParts.length(); j++)
    {
        this->subBodyParts[j]->BP_TranslateMatrix = BP_TranslateMatrix;
        this->subBodyParts[j]->updateTranslationMatrixForAllSubBPs(BP_TranslateMatrix);
    }
}

void BodyPart::updateRotQuatForAllSubBPs(QQuaternion BP_OrientationQuat)
{
//    this->BP_OrientationQuat *= BP_OrientationQuat;
    for (int j=0; j<this->subBodyParts.length(); j++)
    {
        this->subBodyParts[j]->BP_OrientationQuat *= QQuaternion::fromDirection(this->subBodyParts[j]->getPivotVector(),
                                                                                this->subBodyParts[j]->cellList[0]->cellBox->normals[4]);
//        this->subBodyParts[j]->BP_OrientationQuat *= BP_OrientationQuat;

        this->subBodyParts[j]->updateRotQuatForAllSubBPs(BP_OrientationQuat);
    }
}

bool BodyPart::deleteAllBP_Cells(bool leaveFirCell)
{
    qDebug() << "About to delete all cells";
    if (!leaveFirCell) // delete all cells and don't leave the first cell
    {
        QVector<Cell*> cellsToDelete; cellsToDelete.clear();
        for (int k=0; k<cellList.length(); k++)
        {
            Cell *tempCellPtr = cellList[k];
            cellsToDelete.append(tempCellPtr);
            // visitCellsRecursively(tempCellPtr, cellsToDelete);
        }

        if (!cellsToDelete.empty())
        {
            for (int j=0; j<cellsToDelete.length(); j++)
            {
                delete cellsToDelete[j];
            }
            cellsToDelete.clear();
            cellList.clear();

            BP_TranslateMatrix.setToIdentity();
            BP_OrientationQuat = QQuaternion::fromDirection(QVector3D(0,0,1), QVector3D(0,1,0));

            qDebug() << "all cells deleted";
            return true;
        }
        else
        {
            qDebug() << "no cells to be deleted";
            return false;
        }

        // while (cellList.empty()) delete cellList.last(), cellList.removeLast();
        // BP_TranslateMatrix.setToIdentity();
        // BP_OrientationQuat = QQuaternion::fromDirection(QVector3D(0,0,1), QVector3D(0,1,0));

        // return true;
    }
    else if (cellList.length() > 1)// spare the first cell
    {
        QVector<Cell*> cellsToDelete; cellsToDelete.clear();
        for (int k=1; k<cellList.length(); k++)
        {
            Cell *tempCellPtr = cellList[k];
            cellsToDelete.append(tempCellPtr);
            // visitCellsRecursively(tempCellPtr, cellsToDelete);
        }

        if (!cellsToDelete.empty())
        {
            while (!cellsToDelete.empty()) delete cellsToDelete.last(), cellsToDelete.removeLast();
            while (cellList.length() > 1)
                cellList.removeLast();

            qDebug() << "all cells deleted except for the first one";
            return true;
        }
        else
        {
            qDebug() << "no cells to be deleted. This bone might only has one cell i.e. no densifiers";
            return false;
        }
    }

    return false;
}

BodyPart *BodyPart::duplicateBP(Alien *parentAlienPtr, QMap<int, bool> &tempClonedBP_Checklist)
{
    qDebug() << "BodyPart duplicateBP() method called";
}


