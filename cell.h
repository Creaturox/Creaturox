#ifndef CELL_H
#define CELL_H

#include "box.h"

#include <QVector3D>
#include <qopengl.h>
#include <QQuaternion>
#include <QMatrix4x4>


/*
 * A Cell is the most basic component of the alien anatomy. It is represented as a box. It has energy, which can be
 * distributed to newly generated neighboring cells.
 *
 * TODO(ismail): Improvement suggestions:---
 *
 *   isRenderable: There should be a flag in this class to tell the system whether it should be rendered or not.
*/

class Cell
{
public:
    // TODO(ismail): Must have a copy constructor
    Cell();
    Cell(QVector3D centerLoc, float energy, int pivFace);
    ~Cell();
//    Cell(GLfloat cellEnergy, QVector3D cellLocation);
    void makeBox(); // Instantiates the box around the cell's location and computes the geometrical data of the box
                    // This method uses the cell's energy to specify the length and width of the box, which means its length and width are (2 x distance)
                    // The energy of the cell directly impacts the size of the box to be rendered, i.e. greater the energy, greater the size of the box
    void setPivotFace(int pvtFace);
    void calculatePivotPoint();
    void cloneCell(Cell *clonedCell);
    QVector3D getCellCenterLocation();
    QVector3D getPivotPoint();
    QVector3D getPivotVector();
    QVector3D getNormalVectorOfBoxFace(int boxFaceID);
    QVector<QVector3D> getAllVerticesOfQuadFaceInCCwiseOrder(int boxFaceID);
    QVector3D getLocationFrom_u_v_parametersOnBoxFace(int boxFaceID, float uParam, float vParam);
    QVector3D getMidPointOfFace(int face);
    QVector3D getMidPointOfEdge(int face, int edgeID); // This method seems to be buggy.
    QVector<QVector3D> getMirrorPlaneOfOppositeFaces(int origFaceID, int oppFaceID);
    void getAllChildCellsRecursively(QVector<Cell*> &allChildCells);
    void bevelCellBox(quint8 bevelAmnt);

    void deleteBox();   // Unimplemented

    // TODO(ismail): Implement the following methods
    // QuadFace getFace(FaceID)
    // getFaceVertices(FaceID, QVector<QVector3D> &faceVert)
    // QVector3D getFaceNormal(FaceID)


    // TODO(Ismail): REFACTOR: There should be a method for delete box as well.

    int cellID; // TODO(ismail): why we need this? Perhaps not needed
    QVector3D cellCenterLocation; // A 3D point to tell where the cell is generated. By default it is the origin point i.e. (0,0,0)
                            // It is the point which is always at the center of the cell box. Thus its useful to know the location of the cell anytime
                            // May also serves as the cell's unique ID.
    GLfloat cellEnergy;
    // connecting lines OR connectors ==> this line connects this cell with the neighboring cell and are directed along the normals of the box faces
    QVector<Cell*> childCells; // children cells
    Box *cellBox; // The geometry of the cell is represented by a box
    QQuaternion cellOrientation; // used for rotations
    QMatrix4x4 cellTranslateMatrix; // used for translations
    QVector3D cellPivotPt; // A 3D point that serves as the pivot for scalings and perhaps translations
                           // This point is located at the center of the face that serves as the pivot face for the cell
                           // A pivot point can also be used as the originating point for other BPs or cells that are to be attached/originated
                           // ... to this BP/cell
    QVector3D cellPivotVector;   // A 'Pivot Vector' OR 'Orientation Vector' is a vector that is the orientation vector of a box. It is computed as:
                                // ... center point of Front Face of root joint minus center point of back Face of root joint
                                // ... This vector must be updated whenever this BP or its parent BP undergoes rotation
    int pivotFace; // PivotFace can have any of the faces (enum in Box)
    bool isDenser;
};

#endif // CELL_H
