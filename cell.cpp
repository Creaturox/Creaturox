#include "cell.h"
#include <geometryalgorithms.h>

Cell::Cell()
{
    // set default parameters/attributes
    cellTranslateMatrix.setToIdentity(); // initialize translation matrix
    cellOrientation = QQuaternion::fromDirection(QVector3D(0,0,1),QVector3D(0,1,0)); // initialize orientation quaternion
    cellCenterLocation = QVector3D(0,0,0);
    pivotFace = -1; // by default no face is selected for the pivotFace.
    cellPivotPt = QVector3D(0,0,0); // initialize the pivot point
    isDenser = false;
//    cellEnergy = 0;
//    cellBox = NULL;
    //    childCells.resize(0);
}

Cell::Cell(QVector3D centerLoc, float energy, int pivFace)
{
    // set default parameters/attributes
    cellTranslateMatrix.setToIdentity(); // initialize translation matrix
//    cellOrientation = QQuaternion::fromDirection(QVector3D(0,0,1),QVector3D(0,1,0)); // initialize orientation quaternion
    cellCenterLocation = centerLoc;
    pivotFace = pivFace; // by default no face is selected for the pivotFace.
    cellEnergy = energy;
    cellPivotPt = centerLoc; // initialize the pivot point
    cellBox = nullptr;
}

Cell::~Cell()
{
//    qDebug() << "deleting Cell";
    while (!childCells.empty()) delete childCells.last(), childCells.removeLast();
//    childCells.clear();
//    delete &childCells;
    delete cellBox;
//    qDebug() << "Cell deleted";
}

void Cell::makeBox()
{
    cellBox = new Box(cellCenterLocation, cellEnergy);
    QVector3D displacementVector = getCellCenterLocation() - QVector3D(0,0,0);
    // Here we are only updating the translation matrix for this cell. Can also use GeometricAlgorithms::TranslateModel method.
    cellTranslateMatrix.translate(displacementVector);
    calculatePivotPoint();
}

void Cell::setPivotFace(int pvtFace)
{
    pivotFace = pvtFace;
    calculatePivotPoint();
}

void Cell::calculatePivotPoint()
{
    if (pivotFace != -1)
    {
        // adjust pivot point
        QVector3D displacementVector = getCellCenterLocation() - getPivotPoint();
//        GeometryAlgorithms::translateCell(this, displacementVector.x(), displacementVector.y(), displacementVector.z());
        GeometryAlgorithms::translateCell(this, displacementVector);
    }
    else
    {
        cellPivotPt = getCellCenterLocation();
    }

//    if (pivotFace==1)
//    {
//        cellPivotPt = cellBox->vertices[cellBox->faces[Box::BoxFace::BACK].vertNorIndices[0]]
//                  +
//                  (0.5 * (cellBox->vertices[cellBox->faces[Box::BoxFace::BACK].vertNorIndices[2]]
//                  - cellBox->vertices[cellBox->faces[Box::BoxFace::BACK].vertNorIndices[0]]));

//        qDebug () << "pivot: " << cellPivotPt;

//        QVector3D displacementVector = cellCenterLocation - cellPivotPt;

//        GeometryAlgorithms::translateModel(this, displacementVector.x(), displacementVector.y(), displacementVector.z());
//    }
//    else // pivot remains at the center of the cell
//    {
//        cellPivotPt = cellBox->vertices[cellBox->faces[Box::BoxFace::BACK].vertNorIndices[0]]
//                  +
//                  (0.5 * (cellBox->vertices[cellBox->faces[Box::BoxFace::FRONT].vertNorIndices[2]]
//                  - cellBox->vertices[cellBox->faces[Box::BoxFace::BACK].vertNorIndices[0]]));
//    }
}

QVector3D Cell::getCellCenterLocation()
{
    cellCenterLocation = cellBox->vertices[cellBox->faces[Box::BoxFace::BACK]->vertNorIndices[1]]
              +
              (0.5 * (cellBox->vertices[cellBox->faces[Box::BoxFace::FRONT]->vertNorIndices[2]]
              - cellBox->vertices[cellBox->faces[Box::BoxFace::BACK]->vertNorIndices[1]]));

    return cellCenterLocation;
}

QVector3D Cell::getPivotPoint()
{
    if (pivotFace != -1)
    {
        cellPivotPt = cellBox->vertices[cellBox->faces[pivotFace]->vertNorIndices[0]]
                  +
                  (0.5 * (cellBox->vertices[cellBox->faces[pivotFace]->vertNorIndices[2]]
                  - cellBox->vertices[cellBox->faces[pivotFace]->vertNorIndices[0]]));
    }
    else
    {
        cellPivotPt = getCellCenterLocation();
    }

    return cellPivotPt;
}

QVector3D Cell::getPivotVector()
{
    cellPivotVector = cellBox->getPivotVector();
    return cellPivotVector;
}

QVector3D Cell::getNormalVectorOfBoxFace(int boxFaceID)
{
    return this->cellBox->getNormalVectorOfBoxFace(boxFaceID);
}

QVector<QVector3D> Cell::getAllVerticesOfQuadFaceInCCwiseOrder(int boxFaceID)
{
    QVector<QVector3D> allVertices; allVertices.clear();
    for (int i=0; i<QuadFace::FaceVerticesNormalsIndicesEnum::NORMAL; i++)
        allVertices << cellBox->vertices[cellBox->faces[boxFaceID]->vertNorIndices[i]];

    return allVertices;
}

QVector3D Cell::getLocationFrom_u_v_parametersOnBoxFace(int boxFaceID, float uParam, float vParam)
{
    // By default we will get the mid point of this face
    QVector3D finalLoc = getMidPointOfFace(boxFaceID);

    // First get all the vertices in cc-wise fashion
    QVector<QVector3D> allVertices = getAllVerticesOfQuadFaceInCCwiseOrder(boxFaceID);
    // Now find the location point
    QVector3D vec1 = allVertices.at(QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT)
                     - allVertices.at(QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT);
    QVector3D vec2 = allVertices.at(QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT)
                     - allVertices.at(QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT);
    // Find the point accross vec1
    QVector3D pnt1 = allVertices.at(QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT) + ((uParam/100.f)*vec1);
    finalLoc = pnt1 + ((vParam/100.f)*vec2); // Final Point

    return finalLoc;
}

QVector3D Cell::getMidPointOfFace(int face)
{
    return cellBox->vertices[cellBox->faces[face]->vertNorIndices[0]]
              +
              (0.5 * (cellBox->vertices[cellBox->faces[face]->vertNorIndices[2]]
              - cellBox->vertices[cellBox->faces[face]->vertNorIndices[0]]));

}

QVector3D Cell::getMidPointOfEdge(int face, int edgeID)
{
    int ver1_Ind = cellBox->faces[face]->vertNorIndices[cellBox->faces[face]->edgeList[edgeID]->vertexInd_1];
    int ver2_Ind = cellBox->faces[face]->vertNorIndices[cellBox->faces[face]->edgeList[edgeID]->vertexInd_2];

    return cellBox->vertices[cellBox->faces[face]->vertNorIndices[ver1_Ind]]
              +
              (0.5 * (cellBox->vertices[cellBox->faces[face]->vertNorIndices[ver2_Ind]]
              - cellBox->vertices[cellBox->faces[face]->vertNorIndices[ver1_Ind]]));

}

QVector<QVector3D> Cell::getMirrorPlaneOfOppositeFaces(int origFaceID, int oppFaceID)
{
    QVector<QVector3D> threePlanePts; threePlanePts.clear();
    // get the middle plane of this cell box
    QuadFace *origFace = cellBox->faces[origFaceID];
    QuadFace *oppFace = cellBox->faces[oppFaceID];

    QVector3D plVer1 = cellBox->vertices[origFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]] +
            (0.5 * (cellBox->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]
            - cellBox->vertices[origFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]]));

    QVector3D plVer2 = cellBox->vertices[origFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]] +
            (0.5 * (cellBox->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]]
            - cellBox->vertices[origFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]));

    QVector3D plVer3 = cellBox->vertices[origFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]] +
            (0.5 * (cellBox->vertices[oppFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]]
            - cellBox->vertices[origFace->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]));

    threePlanePts << plVer1 << plVer2 << plVer3;
    // Now scale the plane so as should have a big mirror to get the mirroring alright
    return threePlanePts;
}

void Cell::getAllChildCellsRecursively(QVector<Cell*> &allChildCells)
{
    if (!allChildCells.contains(this))
        allChildCells.append(this);
    // go through all the children of this cell and render them recursively
    for (int i=0; i<this->childCells.length(); i++)
    {
        this->childCells[i]->getAllChildCellsRecursively(allChildCells);
    }
}

void Cell::bevelCellBox(quint8 bevelAmnt)
{
    cellBox->bevelBox(bevelAmnt);
}

void Cell::cloneCell(Cell *clonedCell)
{
//    qDebug() << "Cloning a cell";
    // clone the main attributes
    clonedCell->cellCenterLocation = this->getCellCenterLocation();
    clonedCell->cellEnergy = this->cellEnergy;
    clonedCell->cellOrientation = this->cellOrientation;
    clonedCell->cellPivotPt = this->getPivotPoint();
    clonedCell->cellTranslateMatrix = this->cellTranslateMatrix;
    clonedCell->pivotFace = this->pivotFace;
    clonedCell->cellPivotVector = this->getPivotVector();

    // clone the box
    Box *tempNewBox = new Box();
    this->cellBox->cloneBox(tempNewBox);
    clonedCell->cellBox = tempNewBox;

    // clone child cells (if any)
    for (int i=0; i<this->childCells.length(); i++)
    {
        // clone this sub cell
        Cell *tempNewSubCellPtr = new Cell();
        clonedCell->childCells.append(tempNewSubCellPtr);
        this->childCells[i]->cloneCell(tempNewSubCellPtr);
    }

//    qDebug() << "Cell cloned successfully";
}
