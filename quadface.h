#ifndef QUADFACE_H
#define QUADFACE_H

#include "edge.h"

#include <QVector>
#include <QVector3D>
#include <QDebug>

/*
 * This class defines a quadrangular face with 4 vertices stored as indices. It also stores a normal index.
 * Objects of this class are used to construct the faces of a box.
*/

class QuadFace
{
public:
    // TODO(ismail): Must have a copy constructor
    // ... and copy assignment operator
    QuadFace();
    QuadFace(int v1, int v2, int v3, int v4, int n);
    // copy constructor
    QuadFace(const QuadFace &faceArg);
    ~QuadFace();

    void cloneQuadFace(QuadFace *qfPr);

    QVector<int> vertNorIndices; // first four values are vertex indices (in cc-wise fashion) and the last is the normal index
    QVector<Edge*> edgeList; // edges stored in cc-wise fashion... bottom, right, top, left. This ordering correspond to the enum FaceEdge

    enum FaceEdge {BOTTOM, RIGHT, TOP, LEFT};
    enum FaceVerticesNormalsIndicesEnum {BOTTOM_LEFT, BOTTOM_RIGHT, TOP_RIGHT, TOP_LEFT, NORMAL};
};

#endif // QUADFACE_H
