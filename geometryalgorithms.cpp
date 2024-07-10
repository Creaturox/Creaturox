#include "geometryalgorithms.h"

GeometryAlgorithms::GeometryAlgorithms()
{

}

void GeometryAlgorithms::translateCell(Cell *cellPtr, QVector3D displacementVec)
{
//    QVector3D translationVec = destPoint - cellPtr->getPivotPoint();

    QMatrix4x4 transMat;
    transMat.setToIdentity();
    transMat.translate(displacementVec.x(), displacementVec.y(), displacementVec.z());

    for (int i=0; i<cellPtr->cellBox->vertices.length(); i++)
    {
        cellPtr->cellBox->vertices[i] = transMat * cellPtr->cellBox->vertices[i];
    }

//    // update the cell's pivot point because it should also move
    cellPtr->cellPivotPt = cellPtr->getPivotPoint();
    // Update the attribute array (tris and normals) of the box
    // TODO(ismail): Perhaps there's no need to call this method inside transformation code. Just call it once before
    //               ... the box is send to the glWidget for rendering.
    cellPtr->cellBox->updateBoxAttribArr();

    // TODO(ismail): Might need to get rid of Translation matrix updation here
    cellPtr->cellTranslateMatrix *= transMat;

}


void GeometryAlgorithms::translateBP(BodyPart *bp, GLfloat xDist, GLfloat yDist, GLfloat zDist)
{
    // Generate a list of all the cells of the BP (that includes cells of its subBPs) - (Similar to the method getBodyPartListCellListForRendering() from
    //                                                                                  ... AlienModeller class, which uses recursion)
    QVector<Cell*> tempCellList;
    tempCellList.resize(0);
    bp->getBodyPartCellList(tempCellList);

    bp->BP_TranslateMatrix.translate(xDist, yDist, zDist);
    // Transalte the entire BP to the new location
    for (int i=0; i<tempCellList.length(); i++)
    {
        translateCell(tempCellList[i], QVector3D(xDist,yDist,zDist));
    }

    // // update the pivot point because it should also move.
    // for (int i=0; i<tempCellList.length(); i++)
    // {
    //     tempCellList[i]->getPivotPoint();
    //     // Update the attribute array (tris and normals) of the box
    //     tempCellList[i]->cellBox->updateBoxAttribArr();
    // }

    // TODO(ismail): update this BPs pivot point as well. This is important because BP_PivotPt plays an important role in many algos
    bp->BP_PivotPt = bp->getPivotPoint();
}

/* OLD METHOD
void GeometryAlgorithms::translateBP(BodyPart *bp, GLfloat xDist, GLfloat yDist, GLfloat zDist)
{
    // Generate a list of all the cells of the BP (that includes cells of its subBPs) - (Similar to the method getBodyPartListCellListForRendering() from
    //                                                                                  ... AlienModeller class, which uses recursion)

    for (int i=0; i<bp->cellList.length(); i++)
    {
//        GeometryAlgorithms::translateCell(bp->cellList[i], QVector3D(xDist, yDist, zDist));
        for (int j=0; j<bp->cellList[i].cellBox->vertices.length(); j++)
        {
            bp->cellList[i].cellBox->vertices[j] = bp->BP_TranslateMatrix.inverted() * bp->cellList[i].cellBox->vertices[j];
        }
//        bp->cellList[i].getPivotPoint();

        // Update the attribute array (tris and normals) of the box
        bp->cellList[i].cellBox->updateBoxAttribArr();
    }
    bp->BP_TranslateMatrix *= bp->BP_TranslateMatrix.inverted();

    for (int i=0; i<bp->subBodyParts.length(); i++)
    {
        translateBP(bp->subBodyParts[i], 0 - bp->subBodyParts[i]->getPivotPoint().x(), 0 - bp->subBodyParts[i]->getPivotPoint().y(), 0 - bp->subBodyParts[i]->getPivotPoint().z());
    }

    QMatrix4x4 transMat;
    transMat.setToIdentity();
    transMat.translate(xDist, yDist, zDist);
    bp->BP_TranslateMatrix *= transMat;

    for (int i=0; i<bp->cellList.length(); i++)
    {
//        GeometryAlgorithms::translateCell(bp->cellList[i], QVector3D(xDist, yDist, zDist));
        for (int j=0; j<bp->cellList[i].cellBox->vertices.length(); j++)
        {
            bp->cellList[i].cellBox->vertices[j] = bp->BP_TranslateMatrix * bp->cellList[i].cellBox->vertices[j];
        }
        bp->cellList[i].getPivotPoint();

        // Update the attribute array (tris and normals) of the box
        bp->cellList[i].cellBox->updateBoxAttribArr();
    }

    for (int i=0; i<bp->subBodyParts.length(); i++)
    {
        translateBP(bp->subBodyParts[i],    xDist - bp->subBodyParts[i]->getPivotPoint().x(),
                                            yDist - bp->subBodyParts[i]->getPivotPoint().y(),
                                            zDist - bp->subBodyParts[i]->getPivotPoint().z());
    }
}
*/

void GeometryAlgorithms::rotateCellUsingEulerAngles(Cell *cellPtr, GLfloat pitch, GLfloat yaw, GLfloat roll)
{
    // bring it back to origin
    for (int i=0; i<cellPtr->cellBox->vertices.length(); i++)
    {
        cellPtr->cellBox->vertices[i] = cellPtr->cellTranslateMatrix.inverted() * cellPtr->cellBox->vertices[i];
    }
    // Update the attribute array (tris and normals) of the box
//    cellPtr->cellBox->updateBoxAttribArr();

    // un-rotate the box back to its original state (using inverse quaternion) i.e. Unrotate the box
    QQuaternion tempQuat = cellPtr->cellOrientation.inverted();
    for (int i=0; i<cellPtr->cellBox->vertices.length(); i++)
    {
        cellPtr->cellBox->vertices[i] = tempQuat.rotatedVector(cellPtr->cellBox->vertices[i]);
    }

    // rotate the box using input angles
    cellPtr->cellOrientation *= QQuaternion::fromEulerAngles(pitch,yaw,roll);
    for (int i=0; i<cellPtr->cellBox->vertices.length(); i++)
    {
        cellPtr->cellBox->vertices[i] = cellPtr->cellOrientation.rotatedVector(cellPtr->cellBox->vertices[i]);
    }
    // Update the attribute array (tris and normals) of the box
//    cellPtr->cellBox->updateBoxAttribArr();

    for (int i=0; i<cellPtr->cellBox->vertices.length(); i++)
    {
        cellPtr->cellBox->vertices[i] = cellPtr->cellTranslateMatrix * cellPtr->cellBox->vertices[i];
    }

    // update the pivot point because it should also move
    cellPtr->cellPivotPt = cellPtr->getPivotPoint();

    // Update the attribute array (tris and normals) of the box
    cellPtr->cellBox->updateBoxAttribArr();
}

void GeometryAlgorithms::rotateCellTowardsVecDir(Cell *cellPtr, const QVector3D &vec)
{
    // bring it back to origin
    for (int i=0; i<cellPtr->cellBox->vertices.length(); i++)
    {
        cellPtr->cellBox->vertices[i] = cellPtr->cellTranslateMatrix.inverted() * cellPtr->cellBox->vertices[i];
    }
    // Update the attribute array (tris and normals) of the box
//    cellPtr->cellBox->updateBoxAttribArr();

    // TODO(ismail): Remove the below commented code as it's probably not needed
    // un-rotate the box back to its original state (using inverse quaternion) i.e. Unrotate the box
    QQuaternion tempQuat = cellPtr->cellOrientation.inverted();
    for (int i=0; i<cellPtr->cellBox->vertices.length(); i++)
    {
        cellPtr->cellBox->vertices[i] = tempQuat.rotatedVector(cellPtr->cellBox->vertices[i]);
    }

    // rotate the box in the input vector direction

    // The below line also works fine but probably not needed
//    cellPtr->cellOrientation = QQuaternion::rotationTo(QVector3D(0,0,1), vec);
    cellPtr->cellOrientation *= QQuaternion::rotationTo(QVector3D(0,0,1), vec);
    for (int i=0; i<cellPtr->cellBox->vertices.length(); i++)
    {
        cellPtr->cellBox->vertices[i] = cellPtr->cellOrientation.rotatedVector(cellPtr->cellBox->vertices[i]);
    }
    // Update the attribute array (tris and normals) of the box
//    cellPtr->cellBox->updateBoxAttribArr();

    // translate it back to its present position
    for (int i=0; i<cellPtr->cellBox->vertices.length(); i++)
    {
        cellPtr->cellBox->vertices[i] = cellPtr->cellTranslateMatrix * cellPtr->cellBox->vertices[i];
    }

    // update the pivot point because it should also move
    cellPtr->cellPivotPt = cellPtr->getPivotPoint();

    // Update the attribute array (tris and normals) of the box
    cellPtr->cellBox->updateBoxAttribArr();
}

void GeometryAlgorithms::rotateBodyPartUsingEulerAngles(BodyPart &bp, GLfloat pitch, GLfloat yaw, GLfloat roll)
{
    // Generate a list of all the cells of the BP (that includes cells of its subBPs) - (Similar to the method getBodyPartListCellListForRendering() from
    //                                                                                  ... AlienModeller class, which uses recursion)

    QVector<Cell*> tempCellList;
    tempCellList.resize(0);
    bp.getBodyPartCellList(tempCellList);

    QVector3D tempPiv = bp.getPivotPoint();
    translateBP(&bp, 0-tempPiv.x(), 0-tempPiv.y(), 0-tempPiv.z());

    // perform rotation according to input Euler Angles
    bp.BP_OrientationQuat = QQuaternion::fromEulerAngles(pitch,yaw,roll);
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = bp.BP_OrientationQuat.rotatedVector(tempCellList[i]->cellBox->vertices[j]);
        }
        tempCellList[i]->cellOrientation *= bp.BP_OrientationQuat;
    }

    translateBP(&bp, tempPiv.x(), tempPiv.y(), tempPiv.z());

    // update the pivot point because it should also move.
    for (int i=0; i<tempCellList.length(); i++)
    {
        tempCellList[i]->getPivotPoint();
        // Update the attribute array (tris and normals) of the box
        tempCellList[i]->cellBox->updateBoxAttribArr();
    }

    // TODO(ismail): update this BPs pivot point as well. This is important because BP_PivotPt plays an important role in many algos
    bp.BP_PivotPt = bp.getPivotPoint();
}

void GeometryAlgorithms::rotateBodyPartTowardsVecDir(BodyPart &bp, const QVector3D &vec)
{
    // Generate a list of all the cells of the BP (that includes cells of its subBPs) - (Similar to the method getBodyPartListCellListForRendering() from
    //                                                                                  ... AlienModeller class, which uses recursion)

    QVector<Cell*> tempCellList;
    tempCellList.resize(0);
    bp.getBodyPartCellList(tempCellList);

    // bring the BP back to origin
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = bp.BP_TranslateMatrix.inverted() * tempCellList[i]->cellBox->vertices[j];
        }
        tempCellList[i]->cellTranslateMatrix *= bp.BP_TranslateMatrix.inverted();
    }

//    bp.updateTranslationMatrixForAllSubBPs(bp.BP_TranslateMatrix.inverted());

    // perform rotation towards the input vector direction
    bp.BP_OrientationQuat *= QQuaternion::rotationTo(bp.getPivotVector(), vec);
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = bp.BP_OrientationQuat.rotatedVector(tempCellList[i]->cellBox->vertices[j]);
        }
        tempCellList[i]->cellOrientation *= bp.BP_OrientationQuat;
    }

//    bp.updateRotQuatForAllSubBPs(bp.BP_OrientationQuat);

    // take it back to its actual location
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = bp.BP_TranslateMatrix * tempCellList[i]->cellBox->vertices[j];            
        }
        tempCellList[i]->cellTranslateMatrix *= bp.BP_TranslateMatrix;
    }

//    bp.updateTranslationMatrixForAllSubBPs(bp.BP_TranslateMatrix);

    // update the pivot point because it should also move.
    for (int i=0; i<tempCellList.length(); i++)
    {
        tempCellList[i]->getPivotPoint();
        // Update the attribute array (tris and normals) of the box
        tempCellList[i]->cellBox->updateBoxAttribArr();
    }

    // TODO(ismail): update this BPs pivot point as well. This is important because BP_PivotPt plays an important role in many algos
    bp.BP_PivotPt = bp.getPivotPoint();
    //    bp.BP_PivotVector = vec;
}

QVector3D GeometryAlgorithms::rotateVec_EulerAngles(QVector3D inVec, float pitch, float yaw, float roll)
{
    QQuaternion tempQuat = QQuaternion::fromEulerAngles(pitch, yaw, roll);
    return tempQuat.rotatedVector(inVec);
}

void GeometryAlgorithms::scaleModel(Box *boxPtr, float xfactor, float yfactor, float zfactor)
{
    QMatrix4x4 scalMat;

    scalMat.setToIdentity();
    scalMat.scale(xfactor, yfactor, zfactor);

    for (int i=0; i<boxPtr->vertices.length(); i++)
    {
        boxPtr->vertices[i] = scalMat * boxPtr->vertices[i];
    }

    // Update the attribute array (tris and normals) of the box
    boxPtr->updateBoxAttribArr();

//    // Once we have updated the vertices, we need to Update/Recompute the normals
    //    recomputeNormals();
}

void GeometryAlgorithms::scaleCell(Cell *cell, float xfactor, float yfactor, float zfactor)
{
    QVector<Cell*> allChildCells; allChildCells.clear();
    cell->getAllChildCellsRecursively(allChildCells);
    // bring it back to origin
    for (Cell *child : allChildCells)
        for (int i=0; i<child->cellBox->vertices.length(); i++)
        {
            child->cellBox->vertices[i] = cell->cellTranslateMatrix.inverted() * child->cellBox->vertices[i];
        }
    // Update the attribute array (tris and normals) of the box
//    cell->cellBox->updateBoxAttribArr();

    // Unrotate the cell box (using inverse quaternion)
    QQuaternion tempQuat = cell->cellOrientation.inverted();
    for (Cell *child : allChildCells)
        for (int i=0; i<child->cellBox->vertices.length(); i++)
        {
            child->cellBox->vertices[i] = tempQuat.rotatedVector(child->cellBox->vertices[i]);
        }

    // perform scaling
    QMatrix4x4 scalMat;

    scalMat.setToIdentity();
    scalMat.scale(xfactor, yfactor, zfactor);
    for (Cell *child : allChildCells)
        for (int i=0; i<child->cellBox->vertices.length(); i++)
        {
            child->cellBox->vertices[i] = scalMat * child->cellBox->vertices[i];
        }
    // Update the attribute array (tris and normals) of the box
//    cell->cellBox->updateBoxAttribArr();


    // rotate the box back
    for (Cell *child : allChildCells)
        for (int i=0; i<child->cellBox->vertices.length(); i++)
        {
            child->cellBox->vertices[i] = cell->cellOrientation.rotatedVector(child->cellBox->vertices[i]);
        }
    // Update the attribute array (tris and normals) of the box
//    cell->cellBox->updateBoxAttribArr();

    // take it back to its actual location
    for (Cell *child : allChildCells)
        for (int i=0; i<child->cellBox->vertices.length(); i++)
        {
            child->cellBox->vertices[i] = cell->cellTranslateMatrix * child->cellBox->vertices[i];
        }

    // update the pivot point because it should also move. TODO(ismail): Insepct this as it may be buggy. The cell pivot point should be at the center of the
    //                                                     ... box and not on the BACK face
    for (Cell *child : allChildCells)
    {
        child->cellPivotPt = child->getPivotPoint();

        // Update the attribute array (tris and normals) of the box
        child->cellBox->updateBoxAttribArr();

    }
}


void GeometryAlgorithms::scaleBP(BodyPart *bp, float xfactor, float yfactor, float zfactor) // This code is working alright
{
    // Generate a list of all the cells of the BP (that includes cells of its subBPs)
    // move the vertices to origin i.e. translate the pivot point to the origin
    // rotate 'pivot vector' of this BP along with all its vertices to lie on the z-axis). A 'Pivot Vector' is a vector that is the orientation vector of a box. It is computed as
    // ... center point of Front Face of root joint - center point of back Face of root joint
    // scale everything up/down
    // rotate everything back according to the original 'pivot vector'
    // translate pivot point (along with everything) back to its original location.


    // Generate a list of all the cells of the BP (that includes cells of its subBPs) - (Similar to the method getBodyPartListCellListForRendering() from
    //                                                                                  ... AlienModeller class, which uses recursion)

    QVector<Cell*> tempCellList;
    tempCellList.resize(0);
    bp->getBodyPartCellList(tempCellList);

    // bring the BP back to origin
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = bp->BP_TranslateMatrix.inverted() * tempCellList[i]->cellBox->vertices[j];
        }
//        tempCellList[i]->cellTranslateMatrix *= bp->BP_TranslateMatrix.inverted();
    }

//    bp->updateTranslationMatrixForAllSubBPs(bp->BP_TranslateMatrix.inverted());
    // Update the attribute array (tris and normals) of the box
//    cell->cellBox->updateBoxAttribArr();

    // Unrotate the BP (using inverse quaternion)
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = bp->BP_OrientationQuat.inverted().rotatedVector(tempCellList[i]->cellBox->vertices[j]);
        }
//        tempCellList[i]->cellOrientation *= bp->BP_OrientationQuat.inverted();
    }

//    bp->updateRotQuatForAllSubBPs(bp->BP_OrientationQuat.inverted());

//    for (int i=0; i<tempCellList.length(); i++)
//    {
//        tempCellList[i]->getPivotPoint();
//        // Update the attribute array (tris and normals) of the box
//        tempCellList[i]->cellBox->updateBoxAttribArr();
//    }


    // perform scaling
    QMatrix4x4 scalMat;

    scalMat.setToIdentity();
    scalMat.scale(xfactor, yfactor, zfactor);

    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = scalMat * tempCellList[i]->cellBox->vertices[j];
        }
    }

    bp->BP_ScalingMatrix = scalMat;

    // rotate the box back
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = bp->BP_OrientationQuat.rotatedVector(tempCellList[i]->cellBox->vertices[j]);
        }
//        tempCellList[i]->cellOrientation *= bp->BP_OrientationQuat;
    }
//    bp->updateRotQuatForAllSubBPs(bp->BP_OrientationQuat);

    // take it back to its actual location
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = bp->BP_TranslateMatrix * tempCellList[i]->cellBox->vertices[j];
        }
//        tempCellList[i]->cellTranslateMatrix *= bp->BP_TranslateMatrix;
    }

//    bp->updateTranslationMatrixForAllSubBPs(bp->BP_TranslateMatrix);

    // update the pivot point because it should also move. TODO(ismail): Insepct this as it may be buggy. The cell pivot point should be at the center of the
    //                                                     ... box and not on the BACK face
    for (int i=0; i<tempCellList.length(); i++)
    {
        tempCellList[i]->getPivotPoint();
        // Update the attribute array (tris and normals) of the box
        tempCellList[i]->cellBox->updateBoxAttribArr();
    }

    // TODO(ismail): update this BPs pivot point as well. This is important because BP_PivotPt plays an important role in many algos
    bp->BP_PivotPt = bp->getPivotPoint();
}


void GeometryAlgorithms::scalePlane(QVector<QVector3D> &plane, float xfactor, float yfactor, float zfactor)
{
    QMatrix4x4 scalMat;

    scalMat.setToIdentity();
    scalMat.scale(xfactor, yfactor, zfactor);

    for (int i=0; i<3; i++) // don't scale the normal vector :-)
    {
        plane[i] = scalMat.map(plane[i]);
    }
}

void GeometryAlgorithms::reflectBodyPartAroundPlane(BodyPart *bpPtr, QVector3D v1, QVector3D v2, QVector3D v3, QVector3D norm)
{
    // Algorithm
    //  Get a vector from the face's 2nd vertex to the 1st vertex, i.e. vec = vertex0 - vertex1;
    //  Get a Quaternion that is rotated from vec to Vec(0,0,1) i.e. the positive Z axis
    //  Use this quaternion to rotate the BP
    //  Translate the BP to the origin
    //  Finally Reflect the BP using the Reflection Matrix

    // Translate the plane along with the BP to the origin
    // Rotate the plane and BP so that its edge (v1,v2) aligns with the Z-axis
    // Reflect the BP around the Z-axis
    // Unrotate the plane along with the BP back to their original orientation
    // Translate the plane and reflected BP back to the plane's original position

    // Construct the reflection matrix along Y-Z axis
    QVector3D pn = QVector3D::normal(v1,v2,v3);
    float d = (-1) * QVector3D::dotProduct(pn, v1);
    QMatrix4x4 reflectionMat = QMatrix4x4((-2*pn.x()*pn.x())+1,   -2*pn.y()*pn.x(),      -2*pn.z()*pn.x(),       0,
                                           -2*pn.x()*pn.y(),     (-2*pn.y()*pn.y())+1,   -2*pn.z()*pn.y(),       0,
                                           -2*pn.x()*pn.z(),      -2*pn.y()*pn.z(),     (-2*pn.z()*pn.z())+1,    0,
                                           -2*pn.x()*d,           -2*pn.y()*d,           -2*pn.z()*d,            1);

    // Get all cells of the BP
    QVector<Cell*> tempCellList;
    tempCellList.resize(0);
    bpPtr->getBodyPartCellList(tempCellList);

    // reflect the BP
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
//            tempCellList[i]->cellBox->vertices[j] = reflectionMat.map(tempCellList[i]->cellBox->vertices[j]);
            tempCellList[i]->cellBox->vertices[j] = reflectionMat * tempCellList[i]->cellBox->vertices[j];
        }
    }

    for (int i=0; i<tempCellList.length(); i++)
    {
        tempCellList[i]->getPivotPoint();
        // Update the attribute array (tris and normals) of the box
        // TODO(ismail): Doesn't seem to work well. Doesn't get lit up
        tempCellList[i]->cellBox->updateBoxAttribArr(true);
    }

    // TODO(ismail): update this BPs pivot point as well. This is important because BP_PivotPt plays an important role in many algos
    bpPtr->BP_PivotPt = bpPtr->getPivotPoint();


/*
    QVector3D tempVec =  v1 - v2;
    QVector3D tempPivotVec = bpPtr->getPivotVector();
    // First rotate the tempVec towards Z axis and then rotate the BP pivot vector according to the Quat
    tempPivotVec = QQuaternion::rotationTo(tempVec, QVector3D(0,0,1)).rotatedVector(tempPivotVec);
    GeometryAlgorithms::rotateBodyPartTowardsVecDir(*bpPtr,tempPivotVec);
//    GeometryAlgorithms::translateBP(bpPtr, 0.0 - bpPtr->getPivotPoint().x(), 0.0 - bpPtr->getPivotPoint().y(), 0.0 - bpPtr->getPivotPoint().z());
    // Construct the reflection matrix along Y-Z axis
    QMatrix4x4 reflectionMat = QMatrix4x4(-1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1);

    // Get all cells of the BP
    QVector<Cell*> tempCellList;
    tempCellList.resize(0);
    bpPtr->getBodyPartCellList(tempCellList);

    // reflect the BP along Y-Z plane
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = reflectionMat.map(tempCellList[i]->cellBox->vertices[j]);
        }
    }

    for (int i=0; i<tempCellList.length(); i++)
    {
        tempCellList[i]->getPivotPoint();
        // Update the attribute array (tris and normals) of the box
        // TODO(ismail): Doesn't seem to work well. Doesn't get lit up
        tempCellList[i]->cellBox->updateBoxAttribArr();
    }

    // TODO(ismail): update this BPs pivot point as well. This is important because BP_PivotPt plays an important role in many algos
    bpPtr->BP_PivotPt = bpPtr->getPivotPoint();
*/



}

void GeometryAlgorithms::reflectCellListAroundPlane(QVector<Cell *> &tempCellList, QVector3D v1, QVector3D v2, QVector3D v3, QVector3D norm)
{
    // Construct the reflection matrix along Y-Z axis
    QVector3D pn = QVector3D::normal(v1,v2,v3);
    float d = (-1) * QVector3D::dotProduct(pn, v1);
    QMatrix4x4 reflectionMat = QMatrix4x4((-2*pn.x()*pn.x())+1,   -2*pn.y()*pn.x(),      -2*pn.z()*pn.x(),       0,
                                          -2*pn.x()*pn.y(),     (-2*pn.y()*pn.y())+1,   -2*pn.z()*pn.y(),       0,
                                          -2*pn.x()*pn.z(),      -2*pn.y()*pn.z(),     (-2*pn.z()*pn.z())+1,    0,
                                          -2*pn.x()*d,           -2*pn.y()*d,           -2*pn.z()*d,            1);

    // Get all cells of the BP
    // reflect the BP
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            // tempCellList[i]->cellBox->vertices[j] = reflectionMat.map(tempCellList[i]->cellBox->vertices[j]);
            tempCellList[i]->cellBox->vertices[j] = reflectionMat * tempCellList[i]->cellBox->vertices[j];
        }
    }

    for (int i=0; i<tempCellList.length(); i++)
    {
        tempCellList[i]->getPivotPoint();
        // Update the attribute array (tris and normals) of the box
        // TODO(ismail): Doesn't seem to work well. Doesn't get lit up
        tempCellList[i]->cellBox->updateBoxAttribArr(true);
    }

    qDebug() << "celllist mirrored";
}

void GeometryAlgorithms::reflectVertexAroundPlane(QVector3D &inVertex, QVector3D v1, QVector3D v2, QVector3D v3, QVector3D norm)
{
    QVector3D pn = QVector3D::normal(v1,v2,v3);
    float d = (-1) * QVector3D::dotProduct(pn, v1);
    QMatrix4x4 reflectionMat = QMatrix4x4((-2*pn.x()*pn.x())+1,   -2*pn.y()*pn.x(),      -2*pn.z()*pn.x(),       0,
                                           -2*pn.x()*pn.y(),     (-2*pn.y()*pn.y())+1,   -2*pn.z()*pn.y(),       0,
                                           -2*pn.x()*pn.z(),      -2*pn.y()*pn.z(),     (-2*pn.z()*pn.z())+1,    0,
                                           -2*pn.x()*d,           -2*pn.y()*d,           -2*pn.z()*d,            1);

    inVertex = reflectionMat * inVertex;
}

void GeometryAlgorithms::translateBP_AccordingToInputTranslationMat(BodyPart *bpPtr, QMatrix4x4 translationMat)
{
    QVector<Cell*> tempCellList;     tempCellList.clear();
    bpPtr->getBodyPartCellList(tempCellList);

    // bring the BP back to origin
    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = translationMat * tempCellList[i]->cellBox->vertices[j];
        }
        tempCellList[i]->cellTranslateMatrix *= translationMat;
    }
}

void GeometryAlgorithms::rotateBP_AccordingToInputRotationMat(BodyPart *bpPtr, QQuaternion quat)
{
    QVector<Cell*> tempCellList; tempCellList.clear();
    bpPtr->getBodyPartCellList(tempCellList);

    // bpPtr->BP_OrientationQuat *= quat;

    for (int i=0; i<tempCellList.length(); i++)
    {
        for (int j=0; j<tempCellList[i]->cellBox->vertices.length(); j++)
        {
            tempCellList[i]->cellBox->vertices[j] = quat.rotatedVector(tempCellList[i]->cellBox->vertices[j]);
        }
        tempCellList[i]->cellOrientation *= quat;
    }
}

void GeometryAlgorithms::updateBP_CellPrimitiveGeometry(BodyPart *bpPtr)
{
    QVector<Cell*> tempCellList; tempCellList.clear();
    bpPtr->getBodyPartCellList(tempCellList);

    for (int i=0; i<tempCellList.length(); i++)
    {
        tempCellList[i]->getPivotPoint();
        // Update the attribute array (tris and normals) of the box
        tempCellList[i]->cellBox->updateBoxAttribArr();
    }
}

//    Return: 0 = disjoint (no intersection)
//            1 = intersection in the unique point
//            2 = the segment lies in the plane
int GeometryAlgorithms::getLinePlaneIntersectionPoint(QVector3D linePt1,
                                                      QVector3D linePt2,
                                                      QVector3D planePt,
                                                      QVector3D planeNormal,
                                                      QVector3D &intersectionPt)
{
    QVector3D    uVec = linePt2 - linePt1;
    QVector3D    wVec = linePt1 - planePt;

    GLfloat     D = QVector3D::dotProduct(planeNormal,uVec);
    GLfloat     N = (-1.0) * QVector3D::dotProduct(planeNormal,wVec);

    if (fabs(D) < SMALL_NUM) // segment is parallel to plane
    {
        if (N == 0)                      // segment lies in plane
            return 2;
        else
            return 0;                    // no intersection
    }
    // they are not parallel
    // compute intersect param
    GLfloat sI = N / D;
    if (sI < 0 || sI > 1)
        return 0;                        // no intersection

    intersectionPt =  (sI * uVec) + linePt1;  // compute segment intersect point
    return 1;
}

QVector<QVector3D> GeometryAlgorithms::findIntermediatePntsBetweenTwoGivenPnts(QVector3D pnt1,
                                                                               QVector3D pnt2, QVector<float> percList)
{
    QVector<QVector3D> interPnts; interPnts.clear();
    interPnts.append(pnt1);
    QVector3D tempVec = pnt2 - pnt1;
    for (float perc : percList)
    {
        if (perc < 100)
            interPnts.append(pnt1 + ((perc/100)*tempVec));
    }
    interPnts.append(pnt2);

    return interPnts;
}
