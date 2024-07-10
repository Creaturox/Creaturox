#include "box.h"

Box::Box()
{
    // initialize all the vertices with default values
    initVertices(QVector3D(0.0f, 0.0f, 0.0f), 200.50f); // if .00 is set after the decimal point, the subsequent values are mostly resembling integers

    // set all the faces
    setQuadFaces();

//    qDebug() << "vertices: " << vertices.length() << " "
//             << "normals: " << normals.length() << " "
//             << "faces: " << faces.length() << " ";

    // set the labels

    // set the transformations

    // construct the renderable array
    updateBoxAttribArr();

    //    qDebug() << m_data.length();
}

Box::~Box()
{
//    qDebug() << "deleting Box";
    vertices.clear();
    normals.clear();
    while(!faces.empty()) delete faces.last(), faces.removeLast();
//    for (int i=0; i<faces.length(); i++)
//        delete faces[i];
//    faces.clear();
    m_data.clear();
    subBoxes.clear();
//    parentBox=nullptr;
    boxLabel.clear();
//    qDebug() << "Box deleted";
}

// make a box from the input eight points
Box::Box(QVector<QVector3D> &inputPnts)
{
    // copy all the incoming vertices to the vertex list
    for (int i=0; i<8; i++)
    {
        vertices << inputPnts[i];
    }

    // set all the faces
    setQuadFaces();

//    qDebug() << "vertices: " << vertices.length() << " "
//             << "normals: " << normals.length() << " "
//             << "faces: " << faces.length() << " ";

    // set the labels

    // set the transformations

    // construct the renderable array
    updateBoxAttribArr();

//    qDebug() << m_data.length();
}

Box::Box(QVector3D location, GLfloat distFromLocation)
{
    // initialize all the vertices
    initVertices(location, distFromLocation); // if .00 is set after the decimal point, the subsequent values are mostly resembling integers

    // set all the faces
    setQuadFaces();

//    qDebug() << "vertices: " << vertices.length() << " "
//             << "normals: " << normals.length() << " "
//             << "faces: " << faces.length() << " ";

    // set the labels

    // set the transformations

    // construct the renderable array
    updateBoxAttribArr();

//    qDebug() << m_data.length();
}


// set all the 8 vertices to default values - initialize the box to lie on the centre of the origin
void Box::initVertices(QVector3D location, GLfloat x)
{
    // v1
    GLfloat x1 = -x;
    GLfloat y1 = -x;
    GLfloat z1 =  x;

    // v2
    GLfloat x2 =  x;
    GLfloat y2 = -x;
    GLfloat z2 =  x;

    // v3
    GLfloat x3 =  x;
    GLfloat y3 =  x;
    GLfloat z3 =  x;

    // v4
    GLfloat x4 = -x;
    GLfloat y4 =  x;
    GLfloat z4 =  x;

    // v5
    GLfloat x5 = -x;
    GLfloat y5 = -x;
    GLfloat z5 = -x;

    // v6
    GLfloat x6 =  x;
    GLfloat y6 = -x;
    GLfloat z6 = -x;

    // v7
    GLfloat x7 =  x;
    GLfloat y7 =  x;
    GLfloat z7 = -x;

    // v8
    GLfloat x8 = -x;
    GLfloat y8 =  x;
    GLfloat z8 = -x;

    // All the eight vertices of the box are stored in the 'Vertices' list
    vertices.append(location+QVector3D(x1,y1,z1));
    vertices.append(location+QVector3D(x2,y2,z2));
    vertices.append(location+QVector3D(x3,y3,z3));
    vertices.append(location+QVector3D(x4,y4,z4));
    vertices.append(location+QVector3D(x5,y5,z5));
    vertices.append(location+QVector3D(x6,y6,z6));
    vertices.append(location+QVector3D(x7,y7,z7));
    vertices.append(location+QVector3D(x8,y8,z8));
}

// constructs all the faces of the box
// Suggestion: Create macros/enumerations for naming the faces from their indices
void Box::setQuadFaces()
{
    // face 1 = v1,v2,v3,v4     front face
    makeQuadFace(0,1,2,3);

    // face 2 = v2,v6,v7,v3     left face
    makeQuadFace(1,5,6,2);

    // face 3 = v6,v5,v8,v7     back face
    makeQuadFace(5,4,7,6);

    // face 4 = v5,v1,v4,v8     right face
    makeQuadFace(4,0,3,7);

    // face 5 = v4,v3,v7,v8     top face
    makeQuadFace(3,2,6,7);

    // face 6 = v2,v1,v5,v6     bottom face
    makeQuadFace(1,0,4,5);
}


// constructs a face from the provided vertex indices and also computes the normal and adds it to the currently computed face
void Box::makeQuadFace(int v1, int v2, int v3, int v4)
{
//    QVector3D n = QVector3D::normal(vertices[v1],vertices[v2],vertices[v3]);
    normals.append(QVector3D::normal(vertices[v1],vertices[v2],vertices[v3]));
    faces.append(new QuadFace(v1,v2,v3,v4,normals.length()-1));
}

void Box::updateBoxNormals()
{
    normals.resize(0);

    for (int i=0; i<faces.length(); i++)
    {
        normals.append(QVector3D::normal(   vertices[faces[i]->vertNorIndices[0]],
                                            vertices[faces[i]->vertNorIndices[1]],
                                            vertices[faces[i]->vertNorIndices[2]]
                                        ));
    }
}

void Box::updateBvlBoxNormals()
{
    bevelNormals.resize(0);

    for (int i=0; i<bevelQuadFaces.length(); i++)
    {
        bevelNormals.append(QVector3D::normal(  bevelVertices[bevelQuadFaces[i]->vertNorIndices[0]],
                                                bevelVertices[bevelQuadFaces[i]->vertNorIndices[1]],
                                                bevelVertices[bevelQuadFaces[i]->vertNorIndices[2]]
                                                ));
    }

    // We also need to update the normals of the corner triangle faces i.e. bevelTriFacesVertIndices
    for (int i=0; i<bevelTriFacesVertIndices.length(); i++)
    {
        // normal of this tri
        QVector3D normVec = QVector3D::normal
            (
                bevelVertices[bevelTriFacesVertIndices[i][0]],
                bevelVertices[bevelTriFacesVertIndices[i][1]],
                bevelVertices[bevelTriFacesVertIndices[i][2]]
            );

        bevelNormals.append(normVec);
    }
}

int Box::getAdjacentFaceOfEdge(Edge *inputEdgePtr, int faceToExclude)
{
    // *** Algorithm ***
    // Input: an edge
    // Output: the index of the face the input edge is adjacent to

    // Loop through all the faces of this box
    for (int i=0; i<faces.length(); i++)
    {
        if (i != faceToExclude)
        {
            // go through all the edges of this face and check if any edge of the this face is the same as the input edge
            for (int j=0; j<faces[i]->edgeList.length(); j++)
            {
                Edge *tempEdgePtr = faces[i]->edgeList[j];
                if (this->areEdgesSame(inputEdgePtr, tempEdgePtr))
                    return i;
            }
        }
    }
    return -1;
}

bool Box::getCommonVertexBetweenTwoEdges(Edge *edge1, Edge *edge2, QVector3D &commonVx)
{
    if (vertices[edge1->vertexInd_1] == vertices[edge2->vertexInd_1]  ||  vertices[edge1->vertexInd_2] == vertices[edge2->vertexInd_1])
    {
        commonVx = 1 * vertices[edge2->vertexInd_1];
        return true;
    }
    else if (vertices[edge1->vertexInd_1] == vertices[edge2->vertexInd_2]  ||  vertices[edge1->vertexInd_2] == vertices[edge2->vertexInd_2])
    {
        commonVx = 1 * vertices[edge2->vertexInd_2];
        return true;
    }
    else
        return false;
}

bool Box::areEdgesSame(Edge *edge1, Edge *edge2)
{
    if ((vertices[edge1->vertexInd_1] == vertices[edge2->vertexInd_1]  &&  vertices[edge1->vertexInd_2] == vertices[edge2->vertexInd_2])
       ||
       (vertices[edge1->vertexInd_2] == vertices[edge2->vertexInd_1]  &&  vertices[edge1->vertexInd_1] == vertices[edge2->vertexInd_2]))
        return true;
    else
        return false;
}

Edge *Box::duplicateEdge(Edge *inputEdge)
{
    Edge *newEdgePtr = new Edge();

    newEdgePtr->vertexInd_1 = inputEdge->vertexInd_1;
    newEdgePtr->vertexInd_2 = inputEdge->vertexInd_2;

    return newEdgePtr;
}

void Box::cloneBox(Box *clonedBox)
{
/*
    QVector<QVector3D> vertices; // vertex list: All the 8 vertices of the box
    QVector<QVector3D> normals; // normal list: all the normals of the box as unit vectors
    QVector<QuadFace> faces; // face list. The indices correspond to the values in the enum BoxFace
    QVector<GLfloat> m_data; // this is our complete data for BoxyMuscle geometry including vertices and normals.

    QVector<Box*> subBoxes; // children
    Box *parentBox; // pointer to the parent box. Do I need this?
    QVector<int> boxLabel; // three int values representing the entire label...this is experimental. Need to think of a label that can be used in later algorithms
*/

    clonedBox->vertices.clear();
    clonedBox->normals.clear();
    clonedBox->faces.clear();
    clonedBox->m_data.clear();
    clonedBox->subBoxes.clear();


    for (int i=0; i<this->vertices.length(); i++)
    {
        clonedBox->vertices.append(QVector3D(this->vertices[i].x(), this->vertices[i].y(), this->vertices[i].z()));
    }

    for (int i=0; i<this->normals.length(); i++)
    {
        clonedBox->normals.append(QVector3D(this->normals[i].x(), this->normals[i].y(), this->normals[i].z()));
    }

    for (int i=0; i<this->faces.length(); i++)
    {
        QuadFace *qf = new QuadFace();
        this->faces[i]->cloneQuadFace(qf);
        clonedBox->faces.append(qf);
    }

    for (int i=0; i<this->m_data.length(); i++)
    {
        clonedBox->m_data.append(this->m_data[i]);
    }

    // clone the sub boxes (if any)
    for (int i=0; i<this->subBoxes.length(); i++)
    {
        Box *newSubBoxPtr = new Box();
        this->subBoxes[i]->cloneBox(newSubBoxPtr);
        clonedBox->subBoxes.append(newSubBoxPtr);
    }
}

QVector3D Box::getPivotVector()
{
    computePivotVector();
    return boxPivotVector;
}

void Box::computePivotVector()
{
    boxPivotVector = normals[faces[Box::BoxFace::FRONT]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::NORMAL]];
}

bool Box::doesMouseHitThisBox(QVector3D mouseHitPt)
{
    float dist1, dist2;
    // go through all the faces and check if abs(dist(mouseHitPt, face)) < 0.0001
    for (int i=0; i<faces.length(); i++)
    {
//        dist1 = mouseHitPt.distanceToPlane(
//                                                vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]],
//                                                vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]],
//                                                vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]
//                                               );

//        dist2 = mouseHitPt.distanceToPlane(
//                                                vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]],
//                                                vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]],
//                                                vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]
//                                               );

//        qDebug() << "dist1 : " << dist1 << ", dist2 : " << dist2;
//        if (fabs(dist1) < 0.9 || fabs(dist2) < 0.9)
//        {
//            qDebug() << "distance to face : " << dist1 << " and " << dist2;
//            return true;
//        }

        // Check all 4 vertices of the each face of the box
//        dist1 = mouseHitPt.distanceToPoint(vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]]);
//        if (dist1 < mouseHitPt.distanceToPoint(vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]))
//            dist1 = mouseHitPt.distanceToPoint(vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]);
//        if (dist1 < mouseHitPt.distanceToPoint(vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]))
//            dist1 = mouseHitPt.distanceToPoint(vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]);
//        if (dist1 < mouseHitPt.distanceToPoint(vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]]))
//            dist1 = mouseHitPt.distanceToPoint(vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]]);

        // prepare a list of all the 4 vertices of this face in cc-wise fashion
        QVector<QVector3D> tempFaceVertices; tempFaceVertices.clear();
        tempFaceVertices    << vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]]
                            << vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]
                            << vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]
                            << vertices[faces[i]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]];

        // Find 100 random points on this face. 4 face vertices are already added to this list,
        // ... as done below. So in total it is 104 points
        QVector<QVector3D> points = QVector<QVector3D>(tempFaceVertices);
        int numOfBranch = 250;
        // Get the midpoint of the face diagonal. It will serve as the face center point
        QVector3D centerPt = tempFaceVertices[0] + (0.5 * (tempFaceVertices[2] - tempFaceVertices[0]));

        // k goes through face vertices in cc-wise fashion. k starts at a random vertex index
        for (int j=0,k=(rand() % 2); j<numOfBranch; j++,k++)
        {
            if (k==3) // go back to the first vertex
                k=0;

            QVector3D tempPt = tempFaceVertices[k] +
                    ( ((rand() % 100)/100.0) * (tempFaceVertices[k+1] - tempFaceVertices[k]) );

            // finally calculate the random point
            QVector3D randomPt = centerPt + (((rand() % 100)/100.0) * (tempPt - centerPt));
            points.append(randomPt);

            // cycle to the next edge
        }

        // Now Check against all the random points, whether the mouseHitPt is really close to any
        // ... of these points
        dist1 = mouseHitPt.distanceToPoint(points[0]);
        for (int j=1; j<points.length(); j++)
        {
            if (dist1 > mouseHitPt.distanceToPoint(points[j]))
                dist1 = mouseHitPt.distanceToPoint(points[j]);
        }

        if (dist1 < 5)
        {
            qDebug() << "distance to face : " << dist1 << " and " << dist2;
            points.clear();
            return true;
        }
    }
    qDebug() << "distance to face : " << dist1 << " and " << dist2;
    return false;
}

QVector3D Box::getNormalVectorOfBoxFace(int boxFaceID)
{
    return this->normals[faces[boxFaceID]->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::NORMAL]];
}


void Box::bevelBox(quint8 bevelAmnt)
{

    /* Algo:
     *
     * - Find a point using the percentage of distance traveled from one to vertex of a face to opposite
     *   ... vertex of the same face. Ther percentage can be obtained from bevelAmnt
     *
     * - Store the origin vertex as a key for a new entry in a QMultiMap. The value will be the newly computed vertex
     * ... that will form as one of the corner vertices of the Triangles
     *
     * -
     *
    */

    qDebug() << "Commencing Box bevelling";
    bevelTriFacesVertIndices.resize(8);
    // key(int) is the corner vertex index, while the QVector3D is the newly found vertex if the bevelled triangle
    QMultiMap<int,int> cornerVertToTriVerticesMap; cornerVertToTriVerticesMap.clear();
    QMap<int,int> bvlVerToFaceMap; bvlVerToFaceMap.clear();
    QMultiMap<int,QPair<int,int>> faceToCC_VertPairMap; faceToCC_VertPairMap.clear();
    QMultiMap<int,int> fourElemFaceToVertMap; fourElemFaceToVertMap.clear();
    if (bevelAmnt > 30) // This must not be greater than 30
       bevelAmnt = 30;

    QVector<QuadFace*> tempBevelQuadFaces; tempBevelQuadFaces.clear();

    // First of all we need to skrink the 6 main quad faces.
    // This loop updates the corner vertices of the main faces of the box. We call these beveled quad faces
    // It does not yet find the actual beveled faces i.e. those faces that are bridging the two original faces of the beveled box
    for (int i=0; i<this->faces.length(); i++)
    {
        QuadFace *face = this->faces[i];
        // Get the diagonal vector
        QVector3D diagVec = this->vertices[face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]
               - this->vertices[face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]];

        // Now get a new point at bevelAmnt % of the length of the above vector on either sides of the vector
        QVector3D newTriV_1 = vertices[face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT]]
               + ((bevelAmnt/100.f) * diagVec);
        // store this new point in the multimap
        cornerVertToTriVerticesMap.insert(face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_LEFT], (i*4)+0);

        QVector3D newTriV_2 = this->vertices[face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT]]
               + ( (bevelAmnt/100.f) * ((-1)*diagVec) );
        cornerVertToTriVerticesMap.insert(face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_RIGHT], (i*4)+2);

        // do the same for the other diagonal
        diagVec = this->vertices[face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]]
               - this->vertices[face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]];

        QVector3D newTriV_3 = this->vertices[face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT]]
               + ((bevelAmnt/100.f) * diagVec);
        cornerVertToTriVerticesMap.insert(face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::BOTTOM_RIGHT], (i*4)+1);

        QVector3D newTriV_4 = this->vertices[face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT]]
               + ( (bevelAmnt/100.f) * ((-1)*diagVec) );
        cornerVertToTriVerticesMap.insert(face->vertNorIndices[QuadFace::FaceVerticesNormalsIndicesEnum::TOP_LEFT], (i*4)+3);

        // store these as new bevelled quad faces in bevelQuadFaces
        bevelVertices << newTriV_1 << newTriV_3 << newTriV_2 << newTriV_4;
        bevelNormals << normals[face->vertNorIndices[4]]; // we'll copy the normal as it remains the same as original quadface
        bevelQuadFaces.append(new QuadFace((i*4)+0, (i*4)+1, (i*4)+2, (i*4)+3, i));

        // update the bvlVerToFaceMap by inserting 'vert' to 'face' entry
        for (auto a=0; a<4; a++)
            bvlVerToFaceMap.insert((i*4)+a, i);

        faceToCC_VertPairMap.insert(i, QPair<int,int>((i*4)+0, (i*4)+1));
        faceToCC_VertPairMap.insert(i, QPair<int,int>((i*4)+1, (i*4)+2));
        faceToCC_VertPairMap.insert(i, QPair<int,int>((i*4)+2, (i*4)+3));
        faceToCC_VertPairMap.insert(i, QPair<int,int>((i*4)+3, (i*4)+0));
    }

    qDebug() << "cornVertToTriVerticesMap : " << cornerVertToTriVerticesMap;

    // After this loop the cornVertToTriVerticesMap will have 24 entries
    // Now we need to find the newly created quads
    // We will go through vertices one by one and extract its 3 newly generated tri faces...from the previous step
    QList<QPair<int,int>> tempVertPairs; tempVertPairs.clear();
    for (int i=0; i<this->vertices.length(); i++)
    {
       QList<int> vList1 = cornerVertToTriVerticesMap.values(i);
        if (vList1.length() == 3)
        {
            tempVertPairs   << QPair<int,int>(vList1[0], vList1[1])
                            << QPair<int,int>(vList1[1], vList1[2])
                            << QPair<int,int>(vList1[2], vList1[0]);
            vList1.clear();
        }
        else
        {
            qDebug() << "Something's wrong";
            exit(0);
        }
    }
    // qDebug() << "tempVertPairs : " << tempVertPairs;
    qDebug() << "bvlVerToFaceMap : " << bvlVerToFaceMap;
    // qDebug() << "faceToCC_VertPairMap : " << faceToCC_VertPairMap;
    // exit(0);

    QVector<QuadFace*> newBeveledQuadFaces; newBeveledQuadFaces.clear();
    while (!tempVertPairs.isEmpty())
    {
        QPair<int,int> tempPair = tempVertPairs.first();
        // qDebug() << "tempPair : " << tempPair;
        if (fourElemFaceToVertMap.isEmpty())
        {
            fourElemFaceToVertMap.insert(bvlVerToFaceMap.value(tempPair.first), tempPair.first);
            fourElemFaceToVertMap.insert(bvlVerToFaceMap.value(tempPair.second), tempPair.second);
            tempVertPairs.removeFirst();
        }
        else
        {
            if (fourElemFaceToVertMap.contains(bvlVerToFaceMap.value(tempPair.first))  &&  fourElemFaceToVertMap.contains(bvlVerToFaceMap.value(tempPair.second)))
            {
                // we've found the new beveled quad. Now make it cc-wise and append it to the bevelQuadFaces list
                int tempF1 = bvlVerToFaceMap.value(tempPair.first);
                int tempF2 = bvlVerToFaceMap.value(tempPair.second);
                fourElemFaceToVertMap.insert(tempF1, tempPair.first);
                fourElemFaceToVertMap.insert(tempF2, tempPair.second);
                // qDebug() << fourElemFaceToVertMap;

                // cc-wise arrangement
                // The following lists will only capture 2 vertex indices
                QList<int> vertPairForF1 = fourElemFaceToVertMap.values(tempF1);
                QList<int> vertPairForF2 = fourElemFaceToVertMap.values(tempF2);

                // get the cc-wise using tempF1
                QList<QPair<int,int>> tempAllVertPairsForFace = faceToCC_VertPairMap.values(tempF1);
                if (tempAllVertPairsForFace.contains(QPair<int,int>(vertPairForF1[0], vertPairForF1[1])))
                {
                    // Need to reverse the ordering of vertPairForF1 for cc-wise
                    vertPairForF1.append(vertPairForF1[0]);
                    vertPairForF1.removeFirst();
                }
                else if (tempAllVertPairsForFace.contains(QPair<int,int>(vertPairForF1[1], vertPairForF1[0])))
                {
                }
                else
                {
                    qDebug() << "something is wrong";
                    exit(1);
                }

                // get the cc-wise using tempF2
                tempAllVertPairsForFace = faceToCC_VertPairMap.values(tempF2);
                if (tempAllVertPairsForFace.contains(QPair<int,int>(vertPairForF2[0], vertPairForF2[1])))
                {
                    // Need to reverse the ordering of vertPairForF1 for cc-wise
                    vertPairForF2.append(vertPairForF2[0]);
                    vertPairForF2.removeFirst();
                }
                else if (tempAllVertPairsForFace.contains(QPair<int,int>(vertPairForF2[1], vertPairForF2[0])))
                {
                }
                else
                {
                    qDebug() << vertPairForF2;
                    qDebug() << tempAllVertPairsForFace;
                    qDebug() << "something is wrong";
                    exit(2);
                }

                // Find the normal for this face
                bevelNormals.append(QVector3D::normal(
                                                        bevelVertices[vertPairForF1[0]],
                                                        bevelVertices[vertPairForF1[1]],
                                                        bevelVertices[vertPairForF2[0]]
                                                     ));

                // make a new quad face
                tempBevelQuadFaces.append(new QuadFace( vertPairForF1[0],
                                                        vertPairForF1[1],
                                                        vertPairForF2[0],
                                                        vertPairForF2[1],
                                                        bevelNormals.length()-1)); // lastIndexOf() should also work here

                // Algorithm to find the tri beveled face
                // - The above quad face will have vertices in cc-wise.
                // - This means that the middle two vertices will need to be reversed for the tri beveld face
                // - And the third vertex of that tri beveled face needs to append after the above two vertices
                // - First find the third vertex of the beveled tri having the following two vertices
                int tempCornerVer1 = cornerVertToTriVerticesMap.key(vertPairForF1[1]);
                int tempCornerVer2 = cornerVertToTriVerticesMap.key(vertPairForF1[0]);
                if (bevelTriFacesVertIndices[tempCornerVer1].isEmpty())
                {
                    QList<int> tempThreeVertices1 = cornerVertToTriVerticesMap.values(tempCornerVer1);
                    if (tempThreeVertices1.length() == 3)
                    {
                        tempThreeVertices1.removeOne(vertPairForF1[1]);
                        tempThreeVertices1.removeOne(vertPairForF2[0]);
                        // Now we are left with only 1 vertex, which will be the last vertex in the cc-wise sequence
                        // We now have the cc-wise vertices for our beveled tri
                        tempThreeVertices1.prepend(vertPairForF1[1]);
                        tempThreeVertices1.prepend(vertPairForF2[0]);
                        bevelTriFacesVertIndices[tempCornerVer1].append(tempThreeVertices1);
                    }
                }
                if (bevelTriFacesVertIndices[tempCornerVer2].isEmpty())
                {
                    QList<int> tempThreeVertices2 = cornerVertToTriVerticesMap.values(tempCornerVer2);
                    if (tempThreeVertices2.length() == 3)
                    {
                        tempThreeVertices2.removeOne(vertPairForF1[0]);
                        tempThreeVertices2.removeOne(vertPairForF2[1]);
                        // Now we are left with only 1 vertex, which will be the last vertex in the cc-wise sequence
                        // We now have the cc-wise vertices for our beveled tri
                        tempThreeVertices2.prepend(vertPairForF2[1]);
                        tempThreeVertices2.prepend(vertPairForF1[0]);
                        bevelTriFacesVertIndices[tempCornerVer2].append(tempThreeVertices2);
                    }
                }
                // qDebug() << "bevelTriFacesVertIndices : " << bevelTriFacesVertIndices;

                tempVertPairs.removeFirst();
                fourElemFaceToVertMap.clear();
            }
            else
            {
                // put tempPair at the back/last of the list
                tempVertPairs.append(tempPair);
                tempVertPairs.removeFirst();
            }
        }

   }
   // After the above loop we have all our quad faces done. Now we need to do the 8 beveled tris


   // we need to add all these newly found beveled quad faces to the our bevelQuadFaces
   if (tempVertPairs.isEmpty())
   {
       for (int i=0; i<tempBevelQuadFaces.length(); i++)
           bevelQuadFaces.append(tempBevelQuadFaces[i]);

       for (int i=0; i<bevelQuadFaces.length(); i++)
           qDebug() << "bevelQuadFaces : " << bevelQuadFaces[i]->vertNorIndices;
   }
   else
   {
       qDebug() << "tempVertPairs : " << tempVertPairs;
       qDebug() << "Something's wrong";
       exit(7);
   }
   // finally construct the bevelBox_m_data DS for rendering
   updateBvlBoxAttribArr();

   qDebug() << "Box beveled successfully";
   // exit(0);
}


void Box::updateBoxAttribArr(bool boxReflected)
{
    if (boxReflected)
    {
        for (int i=0; i<faces.length(); i++)
        {
            int v1 = faces[i]->vertNorIndices[0];
            int v2 = faces[i]->vertNorIndices[1];
            int v3 = faces[i]->vertNorIndices[2];
            int v4 = faces[i]->vertNorIndices[3];

            faces[i]->vertNorIndices[0] = v2;
            faces[i]->vertNorIndices[1] = v1;
            faces[i]->vertNorIndices[2] = v4;
            faces[i]->vertNorIndices[3] = v3;
        }
    }

    updateBoxNormals();

    m_data.resize(0);

    // go through all the faces
    for (int i=0; i<faces.length(); i++)
    {
        // 1st Triangle
        // v1
        m_data.append(vertices[faces[i]->vertNorIndices[0]].x());
        m_data.append(vertices[faces[i]->vertNorIndices[0]].y());
        m_data.append(vertices[faces[i]->vertNorIndices[0]].z());

        // normal of this tri
        m_data.append(normals[faces[i]->vertNorIndices[4]].x());
        m_data.append(normals[faces[i]->vertNorIndices[4]].y());
        m_data.append(normals[faces[i]->vertNorIndices[4]].z());

        // v2
        m_data.append(vertices[faces[i]->vertNorIndices[1]].x());
        m_data.append(vertices[faces[i]->vertNorIndices[1]].y());
        m_data.append(vertices[faces[i]->vertNorIndices[1]].z());

        // normal of this tri
        m_data.append(normals[faces[i]->vertNorIndices[4]].x());
        m_data.append(normals[faces[i]->vertNorIndices[4]].y());
        m_data.append(normals[faces[i]->vertNorIndices[4]].z());

        // v3
        m_data.append(vertices[faces[i]->vertNorIndices[2]].x());
        m_data.append(vertices[faces[i]->vertNorIndices[2]].y());
        m_data.append(vertices[faces[i]->vertNorIndices[2]].z());

        // normal of this tri
        m_data.append(normals[faces[i]->vertNorIndices[4]].x());
        m_data.append(normals[faces[i]->vertNorIndices[4]].y());
        m_data.append(normals[faces[i]->vertNorIndices[4]].z());


        // 2nd Triangle
        // v3
        m_data.append(vertices[faces[i]->vertNorIndices[2]].x());
        m_data.append(vertices[faces[i]->vertNorIndices[2]].y());
        m_data.append(vertices[faces[i]->vertNorIndices[2]].z());

        // normal of this tri
        m_data.append(normals[faces[i]->vertNorIndices[4]].x());
        m_data.append(normals[faces[i]->vertNorIndices[4]].y());
        m_data.append(normals[faces[i]->vertNorIndices[4]].z());

        // v4
        m_data.append(vertices[faces[i]->vertNorIndices[3]].x());
        m_data.append(vertices[faces[i]->vertNorIndices[3]].y());
        m_data.append(vertices[faces[i]->vertNorIndices[3]].z());

        // normal of this tri
        m_data.append(normals[faces[i]->vertNorIndices[4]].x());
        m_data.append(normals[faces[i]->vertNorIndices[4]].y());
        m_data.append(normals[faces[i]->vertNorIndices[4]].z());

        // v1
        m_data.append(vertices[faces[i]->vertNorIndices[0]].x());
        m_data.append(vertices[faces[i]->vertNorIndices[0]].y());
        m_data.append(vertices[faces[i]->vertNorIndices[0]].z());

        // normal of this tri
        m_data.append(normals[faces[i]->vertNorIndices[4]].x());
        m_data.append(normals[faces[i]->vertNorIndices[4]].y());
        m_data.append(normals[faces[i]->vertNorIndices[4]].z());
    }

//    for (int i=0; i<m_data.length();i+=6)
//    {
//        qDebug() << m_data[i] << " " << m_data[i+1] << " " << m_data[i+2] << " normal: " << m_data[i+3] << " " << m_data[i+4] << " " << m_data[i+5];
    //    }
}

void Box::updateBvlBoxAttribArr(bool bvlBoxReflected)
{
    qDebug() << "commencing updating bvl box attrib array";
    // change the anti-cc-wise sequence, due to reflection
    if (bvlBoxReflected)
    {
        for (int i=0; i<bevelQuadFaces.length(); i++)
        {
            int v1 = bevelQuadFaces[i]->vertNorIndices[0];
            int v2 = bevelQuadFaces[i]->vertNorIndices[1];
            int v3 = bevelQuadFaces[i]->vertNorIndices[2];
            int v4 = bevelQuadFaces[i]->vertNorIndices[3];

            bevelQuadFaces[i]->vertNorIndices[0] = v2;
            bevelQuadFaces[i]->vertNorIndices[1] = v1;
            bevelQuadFaces[i]->vertNorIndices[2] = v4;
            bevelQuadFaces[i]->vertNorIndices[3] = v3;
        }
        // We also need to cater for trianle faces i.e. bevelTriFacesVertIndices
        for (int i=0; i<bevelTriFacesVertIndices.length(); i++)
        {
            bevelTriFacesVertIndices[i].append(bevelTriFacesVertIndices[i][1]);
            bevelTriFacesVertIndices[i].removeAt(1);
        }
    }

    qDebug() << "reached 1";

    updateBvlBoxNormals();

    qDebug() << "reached 2";

    bevelBox_m_data.resize(0);

    // go through all the faces
    for (int i=0; i<bevelQuadFaces.length(); i++)
    {
        // 1st Triangle
        // v1
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[0]].x());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[0]].y());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[0]].z());

        // normal of this tri
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].x());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].y());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].z());

        // v2
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[1]].x());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[1]].y());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[1]].z());

        // normal of this tri
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].x());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].y());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].z());

        // v3
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[2]].x());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[2]].y());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[2]].z());

        // normal of this tri
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].x());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].y());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].z());


        // 2nd Triangle
        // v3
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[2]].x());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[2]].y());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[2]].z());

        // normal of this tri
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].x());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].y());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].z());

        // v4
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[3]].x());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[3]].y());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[3]].z());

        // normal of this tri
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].x());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].y());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].z());

        // v1
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[0]].x());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[0]].y());
        bevelBox_m_data.append(bevelVertices[bevelQuadFaces[i]->vertNorIndices[0]].z());

        // normal of this tri
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].x());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].y());
        bevelBox_m_data.append(bevelNormals[bevelQuadFaces[i]->vertNorIndices[4]].z());
    }

    // finally we need to add the 8 corner triangles that are stored in bevelTriFacesVertIndices
    for (int i=0; i<bevelTriFacesVertIndices.length(); i++)
    {
        // first compute the normal of this tri
        QVector3D normVec = QVector3D::normal
            (
                bevelVertices[bevelTriFacesVertIndices[i][0]],
                bevelVertices[bevelTriFacesVertIndices[i][1]],
                bevelVertices[bevelTriFacesVertIndices[i][2]]
            );

        // v1
        bevelBox_m_data.append(bevelVertices[bevelTriFacesVertIndices[i][0]].x());
        bevelBox_m_data.append(bevelVertices[bevelTriFacesVertIndices[i][0]].y());
        bevelBox_m_data.append(bevelVertices[bevelTriFacesVertIndices[i][0]].z());

        // normal of this tri
        bevelBox_m_data.append(normVec.x());
        bevelBox_m_data.append(normVec.y());
        bevelBox_m_data.append(normVec.z());


        // v2
        bevelBox_m_data.append(bevelVertices[bevelTriFacesVertIndices[i][1]].x());
        bevelBox_m_data.append(bevelVertices[bevelTriFacesVertIndices[i][1]].y());
        bevelBox_m_data.append(bevelVertices[bevelTriFacesVertIndices[i][1]].z());

        // normal of this tri
        bevelBox_m_data.append(normVec.x());
        bevelBox_m_data.append(normVec.y());
        bevelBox_m_data.append(normVec.z());


        // v3
        bevelBox_m_data.append(bevelVertices[bevelTriFacesVertIndices[i][2]].x());
        bevelBox_m_data.append(bevelVertices[bevelTriFacesVertIndices[i][2]].y());
        bevelBox_m_data.append(bevelVertices[bevelTriFacesVertIndices[i][2]].z());

        // normal of this tri
        bevelBox_m_data.append(normVec.x());
        bevelBox_m_data.append(normVec.y());
        bevelBox_m_data.append(normVec.z());
    }

    qDebug() << "bvl box attrib array updated successfully";
}

//bool Box::getCommonVertexBetweenTwoEdges(Edge *edge1, Edge *edge2, QVector3D &commonVx)
//{
//    if (thisBoxPtr->vertices[this->vertexInd_1] == inputBoxPtr->vertices[inputEdge->vertexInd_1]
//            ||
//        thisBoxPtr->vertices[this->vertexInd_1] == inputBoxPtr->vertices[inputEdge->vertexInd_2])
//    {
//        commonVx = 1 * thisBoxPtr->vertices[this->vertexInd_1];
//        return true;
//    }
//    else if (thisBoxPtr->vertices[this->vertexInd_2] == inputBoxPtr->vertices[inputEdge->vertexInd_1]
//                 ||
//             thisBoxPtr->vertices[this->vertexInd_2] == inputBoxPtr->vertices[inputEdge->vertexInd_2])

//    {
//        commonVx = 1 * thisBoxPtr->vertices[this->vertexInd_2];
//        return true;
//    }
//    else
//        return false;
//}

//bool Box::isThisEdgeSameAs(Edge *inputEdge, Box *thisBoxPtr, Box *inputBoxPtr)
//{
//    if ((thisBoxPtr->vertices[this->vertexInd_1] == inputBoxPtr->vertices[inputEdge->vertexInd_1]
//         &&
//         thisBoxPtr->vertices[this->vertexInd_2] == inputBoxPtr->vertices[inputEdge->vertexInd_2])

//            ||

//        (thisBoxPtr->vertices[this->vertexInd_2] == inputBoxPtr->vertices[inputEdge->vertexInd_1]
//         &&
//         thisBoxPtr->vertices[this->vertexInd_1] == inputBoxPtr->vertices[inputEdge->vertexInd_2]))
//        return true;
//    else
//        return false;
//}

//Edge *Box::duplicateEdge()
//{
//    Edge *newEdgePtr = new Edge();

//    newEdgePtr->vertexInd_1 = this->vertexInd_1;
//    newEdgePtr->vertexInd_2 = this->vertexInd_2;

//    return newEdgePtr;
//}
