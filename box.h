#ifndef BOX_H
#define BOX_H

#include "quadface.h"
#include <qopengl.h>

#include <QVector>
#include <QVector3D>
#include <QDebug>
#include <math.h>

/*
 * The box (a simple cube) is the elementary shape in our alien modelling system.
 * Each box is composed of 6 faces naturally
*/

class Box
{
public:
    // TODO(ismail): Must have a copy constructor
    // ... and copy assignment operator
    Box(); // this default constructor makes a hard coded box at the origin and with 200 energy
    // A constructor that takes in a vector of eight 3D points and makes a box out of it.
    Box(QVector<QVector3D> &inputPnts);
    Box(QVector3D location, GLfloat distFromLocation);
    ~Box();
    void initVertices(QVector3D location, GLfloat x); // x represents the value that each vertex will get with specifying the distance from origin
    void setQuadFaces(); // sets all the 6 faces by assigning each face the vertex indices from the 'vertices' list. This function actually calls the makeFace() method 6 times
    void makeQuadFace(int v1, int v2, int v3, int v4); // makes a quad face from the 4 vertices v1, v2, v3, and v4. The normal (should be represented as 'n') is computed from 3 of the vertices
    void updateBoxAttribArr(bool boxReflected = false); // construct the opengl renderable array
    void updateBvlBoxAttribArr(bool bvlBoxReflected = false); // construct the opengl renderable array
    void updateBoxNormals(); // this method should be called to ensure that the box always has the up to date normals
    void updateBvlBoxNormals(); // this method should be called to ensure that the bevelled box always has the up to date normals
    const GLfloat *constData() const
    {
        if (!bevelBox_m_data.isEmpty())
            return bevelBox_m_data.constData();
        else
            return m_data.constData();
    }

    // returns a face index
    // TODO: This should return two faces if the edge is shared by two faces.
    // ... Since for the box, every edge is shared by two faces, so this method must always return two faces
    int getAdjacentFaceOfEdge(Edge* inputEdgePtr, int faceToExclude);
//    void getAdjacentFacesOfFace(); // This would be a very useful method to implement
    bool getCommonVertexBetweenTwoEdges(Edge *edge1, Edge *edge2, QVector3D &commonVx);
    bool areEdgesSame(Edge *edge1, Edge *edge2);
    Edge *duplicateEdge(Edge *inputEdge);

    void cloneBox(Box *clonedBox);

    QVector3D getPivotVector();
    void computePivotVector();

    // TODO(ismail): Implement the following methods
    // QuadFace getFace(FaceID)
    // getFaceVertices(FaceID, QVector<QVector3D> &faceVert)
    // QVector3D getFaceNormal(FaceID)

    bool doesMouseHitThisBox(QVector3D mouseHitPt);
    QVector3D getNormalVectorOfBoxFace(int boxFaceID);
    void bevelBox(quint8 bevelAmnt);


//    bool getCommonVertexWithAnotherEdge(Edge *inputEdge, QVector3D &commonVx, Box *thisBoxPtr, Box *inputBoxPtr);
//    bool isThisEdgeSameAs(Edge *inputEdge, Box *thisBoxPtr, Box *inputBoxPtr);
//    Edge *duplicateEdge();

//    getFaceVertices(QuadFace &face, QVector<QVector3D> &tempFaceVertices);


    // Unimplemented methods
    void setLabel();
    void setSubBoxes();
    void setParent();
//    void updateVertices();
    // some more important methods: local transformations (position, orientation, size). Better add these to GeometryAlgorithms class


    QVector<QVector3D> vertices; // vertex list: All the 8 vertices of the box
    QVector<QVector3D> normals; // normal list: all the normals of the box as unit vectors
    QVector<QuadFace*> faces; // face list. The indices correspond to the values in the enum BoxFace
    // Each quad face is written as its two triangles.
    // Each triangle is written as tri_v1.x, tri_v1.y, tri_v1.z, face_normal.x, face_normal.y, face_normal.z
    QVector<GLfloat> m_data; // this is our complete data for Box geometry. Contains vertices and normals.

    // Bevel Box - Data structures - These include all the vertices, normals and faces including the native box attributes
    // ... as described above
    // These DSs are filled up when the box is bevelled
    QVector<QVector3D> bevelVertices; // vertex list: All the 24 vertices of the beveled box
    QVector<QVector3D> bevelNormals; // normal list: all the 26 normals of the box as unit vectors
    QVector<QuadFace*> bevelQuadFaces; // face list. 18 Quad faces are stored in this DS. The indices correspond to the values in the enum BoxFace
                                   // There are 8 triangular faces of a beveled box, which not stored in this DS cuz we can't do so
    QVector<QVector<int>> bevelTriFacesVertIndices;    // contains 8 tri faces. Each Tri face has 3 vertices stored in the inner list.
    QVector<GLfloat> bevelBox_m_data; // this is our complete data for Beveled Box geometry. Contains vertices and normals.

    QVector<Box*> subBoxes; // children
    QVector3D boxPivotVector;   // A 'Pivot Vector' is a vector that is the orientation vector of a box. It is computed as:
                                // ... center point of Front Face of root joint minus center point of back Face of root joint
                                // ... This vector must be updated whenever the parent BP undergoes rotation

    enum BoxFace {FRONT, LEFT, BACK, RIGHT, TOP, BOTTOM, BoxFace_Length};
//    enum BeveledBoxFace {FRONT, LEFT, BACK, RIGHT, TOP, BOTTOM, FR_LT, FR_RT, FR_TP, FR_BM, BK_LT, FR_RT, FR_TP, FR_BM, LT_BK, RT_BK, TP_LT, TP_RT, BoxFace_Length};

    Box *parentBox; // pointer to the parent box. Do I need this?
    QVector<int> boxLabel; // three int values representing the entire label...this is experimental. Need to think of a label that can be used in later algorithms
                           // TODO(ismail): perhaps not needed


    // Add a boolean value for renderable. true: to be rendered. false otherwise

};

#endif // BOX_H
